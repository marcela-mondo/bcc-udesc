/*
 * port-warden | Gerente SNMP
 *
 * Programa de linha de comando que conversa com o agente SNMP do switch
 * usando a API SNMP4J. É chamado pelo PHP (bloqueio imediato) e pelo
 * crontab (bloqueio/desbloqueio agendado).
 *
 * Comandos:
 *   bloquear    <ip> <ifIndex>   SET ifAdminStatus = 2 (down)
 *   desbloquear <ip> <ifIndex>   SET ifAdminStatus = 1 (up)
 *   consultar   <ip> <ifIndex>   GET de uma porta
 *   status      <ip>             tabela de interfaces (ifDescr, ifType, admin, oper)
 *   macs        <ip>             MAC aprendidos -> porta (BRIDGE-MIB / Q-BRIDGE-MIB)
 *   ajuda
 *
 * Opções (antes ou depois do comando):
 *   -c <community>   padrão: "private" para SET, "public" para leitura
 *   -p <porta>       porta UDP do agente (padrão 161)
 *   -t <ms>          timeout por tentativa (padrão 1500)
 *   -r <n>           tentativas extras (padrão 2)
 *   --simular        não envia SNMP; simula um switch de 24 portas
 *                    (também ativado por PORTWARDEN_SIMULAR=1)
 *
 * Saída:
 *   stdout -> sempre UM objeto JSON (para o PHP fazer json_decode)
 *   stderr -> uma linha de log com data/hora (o cron redireciona para arquivo)
 *
 * Código de saída:
 *   0 = sucesso | 1 = uso incorreto | 2 = erro SNMP (timeout, community, etc.)
 *
 * MIBs usadas (MIB-II e BRIDGE-MIB):
 *   ifDescr       1.3.6.1.2.1.2.2.1.2
 *   ifType        1.3.6.1.2.1.2.2.1.3
 *   ifAdminStatus 1.3.6.1.2.1.2.2.1.7   (1 = up, 2 = down)  <- o que o SET altera
 *   ifOperStatus  1.3.6.1.2.1.2.2.1.8   (1 = up, 2 = down)  <- link físico
 */

import org.snmp4j.CommunityTarget;
import org.snmp4j.PDU;
import org.snmp4j.Snmp;
import org.snmp4j.TransportMapping;
import org.snmp4j.event.ResponseEvent;
import org.snmp4j.mp.SnmpConstants;
import org.snmp4j.smi.Integer32;
import org.snmp4j.smi.OID;
import org.snmp4j.smi.OctetString;
import org.snmp4j.smi.UdpAddress;
import org.snmp4j.smi.Variable;
import org.snmp4j.smi.VariableBinding;
import org.snmp4j.transport.DefaultUdpTransportMapping;
import org.snmp4j.util.DefaultPDUFactory;
import org.snmp4j.util.TableEvent;
import org.snmp4j.util.TableUtils;

import java.io.IOException;
import java.io.InputStream;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.TreeMap;

public class Gerente {

    // ---- OIDs -------------------------------------------------------------
    static final String OID_IF_DESCR  = "1.3.6.1.2.1.2.2.1.2";
    static final String OID_IF_TYPE   = "1.3.6.1.2.1.2.2.1.3";
    static final String OID_IF_ADMIN  = "1.3.6.1.2.1.2.2.1.7";
    static final String OID_IF_OPER   = "1.3.6.1.2.1.2.2.1.8";

    static final String OID_FDB_ADDR   = "1.3.6.1.2.1.17.4.3.1.1"; // dot1dTpFdbAddress
    static final String OID_FDB_PORT   = "1.3.6.1.2.1.17.4.3.1.2"; // dot1dTpFdbPort
    static final String OID_BASE_IFIDX = "1.3.6.1.2.1.17.1.4.1.2"; // dot1dBasePortIfIndex
    static final String OID_QFDB_PORT  = "1.3.6.1.2.1.17.7.1.2.2.1.2"; // dot1qTpFdbPort

    static final int ADMIN_UP = 1;
    static final int ADMIN_DOWN = 2;

    static final int SAIDA_OK = 0;
    static final int SAIDA_USO = 1;
    static final int SAIDA_ERRO_SNMP = 2;

    // ---- Opções -----------------------------------------------------------
    String comando;
    String ip;
    Integer ifIndex;
    String community;
    int portaUdp = 161;
    long timeoutMs = 1500;
    int tentativas = 2;
    boolean simular = "1".equals(System.getenv("PORTWARDEN_SIMULAR"));

    public static void main(String[] args) {
        // Força UTF-8 mesmo sob o cron (que costuma rodar sem locale definido)
        System.setOut(new PrintStream(new FileOutputStream(FileDescriptor.out), true, StandardCharsets.UTF_8));
        System.setErr(new PrintStream(new FileOutputStream(FileDescriptor.err), true, StandardCharsets.UTF_8));
        Gerente g = new Gerente();
        int codigo;
        try {
            g.lerArgumentos(args);
            codigo = g.executar();
        } catch (ErroUso e) {
            imprimir(json("ok", false, "erro", e.getMessage(), "uso", USO_CURTO));
            log("ERRO DE USO: " + e.getMessage());
            codigo = SAIDA_USO;
        } catch (ErroSnmp e) {
            imprimir(json("ok", false, "comando", g.comando, "ip", g.ip,
                    "if_index", g.ifIndex, "erro", e.getMessage()));
            log(g.comando + " " + g.ip + (g.ifIndex != null ? " ifIndex=" + g.ifIndex : "")
                    + " -> FALHA: " + e.getMessage());
            codigo = SAIDA_ERRO_SNMP;
        }
        System.exit(codigo);
    }

    // =======================================================================
    // Argumentos
    // =======================================================================
    static final String USO_CURTO =
            "gerente.jar [-c community] [-t ms] [-r n] [--simular] "
            + "bloquear|desbloquear|consultar <ip> <ifIndex> | status|macs <ip> | ajuda";

    void lerArgumentos(String[] args) throws ErroUso {
        List<String> pos = new ArrayList<>();
        for (int i = 0; i < args.length; i++) {
            String a = args[i];
            switch (a) {
                case "-c": community = valor(args, ++i, a); break;
                case "-p": portaUdp = inteiro(valor(args, ++i, a), a); break;
                case "-t": timeoutMs = inteiro(valor(args, ++i, a), a); break;
                case "-r": tentativas = inteiro(valor(args, ++i, a), a); break;
                case "--simular": simular = true; break;
                default:
                    if (a.startsWith("-") && !a.equals("-h")) throw new ErroUso("opção desconhecida: " + a);
                    pos.add(a);
            }
        }
        if (pos.isEmpty()) throw new ErroUso("informe um comando");
        comando = pos.get(0).toLowerCase(Locale.ROOT);

        switch (comando) {
            case "ajuda": case "help": case "-h":
                comando = "ajuda";
                return;
            case "bloquear": case "desbloquear": case "consultar":
                if (pos.size() != 3) throw new ErroUso(comando + " precisa de <ip> <ifIndex>");
                ip = pos.get(1);
                ifIndex = inteiro(pos.get(2), "ifIndex");
                if (ifIndex <= 0) throw new ErroUso("ifIndex deve ser maior que zero");
                break;
            case "status": case "macs":
                if (pos.size() != 2) throw new ErroUso(comando + " precisa de <ip>");
                ip = pos.get(1);
                break;
            default:
                throw new ErroUso("comando desconhecido: " + comando);
        }
        if (community == null) {
            boolean escrita = comando.equals("bloquear") || comando.equals("desbloquear");
            community = escrita ? "private" : "public";
        }
    }

    static String valor(String[] args, int i, String opcao) throws ErroUso {
        if (i >= args.length) throw new ErroUso("faltou o valor de " + opcao);
        return args[i];
    }

    static int inteiro(String s, String nome) throws ErroUso {
        try {
            return Integer.parseInt(s.trim());
        } catch (NumberFormatException e) {
            throw new ErroUso(nome + " deve ser um número: " + s);
        }
    }

    // =======================================================================
    // Execução
    // =======================================================================
    int executar() throws ErroSnmp {
        if (comando.equals("ajuda")) {
            imprimir(json("ok", true, "uso", USO_CURTO));
            return SAIDA_OK;
        }
        Switch sw = simular ? new SwitchSimulado(ip) : new SwitchSnmp(this);
        try {
            switch (comando) {
                case "bloquear":    return alterar(sw, ADMIN_DOWN);
                case "desbloquear": return alterar(sw, ADMIN_UP);
                case "consultar":   return consultar(sw);
                case "status":      return status(sw);
                case "macs":        return macs(sw);
                default: throw new IllegalStateException(comando);
            }
        } finally {
            sw.fechar();
        }
    }

    /** SET em ifAdminStatus e, em seguida, GET para confirmar. */
    int alterar(Switch sw, int valor) throws ErroSnmp {
        sw.setAdminStatus(ifIndex, valor);
        Map<String, Object> porta = sw.consultar(ifIndex);
        Object lido = porta.get("admin");
        String esperado = nomeStatus(valor);
        if (!esperado.equals(lido)) {
            throw new ErroSnmp("SET aceito, mas a porta continua '" + lido + "'");
        }
        imprimir(json("ok", true, "comando", comando, "ip", ip, "if_index", ifIndex,
                "admin", lido, "oper", porta.get("oper"), "descricao", porta.get("descricao"),
                "simulado", simular));
        log(comando + " " + ip + " ifIndex=" + ifIndex + " -> OK (admin=" + lido + ")"
                + (simular ? " [SIMULADO]" : ""));
        return SAIDA_OK;
    }

    int consultar(Switch sw) throws ErroSnmp {
        Map<String, Object> porta = sw.consultar(ifIndex);
        Map<String, Object> saida = new LinkedHashMap<>();
        saida.put("ok", true);
        saida.put("comando", comando);
        saida.put("ip", ip);
        saida.putAll(porta);
        saida.put("simulado", simular);
        imprimir(toJson(saida));
        log("consultar " + ip + " ifIndex=" + ifIndex + " -> " + porta.get("admin"));
        return SAIDA_OK;
    }

    int status(Switch sw) throws ErroSnmp {
        List<Map<String, Object>> portas = sw.status();
        imprimir(json("ok", true, "comando", comando, "ip", ip, "simulado", simular,
                "total", portas.size(), "portas", portas));
        log("status " + ip + " -> " + portas.size() + " interfaces");
        return SAIDA_OK;
    }

    int macs(Switch sw) throws ErroSnmp {
        List<Map<String, Object>> lista = sw.macs();
        imprimir(json("ok", true, "comando", comando, "ip", ip, "simulado", simular,
                "total", lista.size(), "macs", lista));
        log("macs " + ip + " -> " + lista.size() + " MACs");
        return SAIDA_OK;
    }

    static String nomeStatus(int v) {
        switch (v) {
            case 1: return "up";
            case 2: return "down";
            case 3: return "testing";
            case 5: return "dormant";
            case 6: return "notPresent";
            case 7: return "lowerLayerDown";
            default: return "desconhecido(" + v + ")";
        }
    }

    // =======================================================================
    // Abstração do switch (real ou simulado)
    // =======================================================================
    interface Switch {
        void setAdminStatus(int ifIndex, int valor) throws ErroSnmp;
        Map<String, Object> consultar(int ifIndex) throws ErroSnmp;
        List<Map<String, Object>> status() throws ErroSnmp;
        List<Map<String, Object>> macs() throws ErroSnmp;
        void fechar();
    }

    // ---- Switch real, via SNMP4J -----------------------------------------
    static class SwitchSnmp implements Switch {
        final Snmp snmp;
        final CommunityTarget<UdpAddress> alvo;

        SwitchSnmp(Gerente g) throws ErroSnmp {
            try {
                TransportMapping<UdpAddress> transporte = new DefaultUdpTransportMapping();
                snmp = new Snmp(transporte);
                transporte.listen();
            } catch (IOException e) {
                throw new ErroSnmp("não foi possível abrir socket UDP: " + e.getMessage());
            }
            alvo = new CommunityTarget<>();
            alvo.setCommunity(new OctetString(g.community));
            alvo.setAddress(new UdpAddress(g.ip + "/" + g.portaUdp));
            alvo.setVersion(SnmpConstants.version2c);
            alvo.setTimeout(g.timeoutMs);
            alvo.setRetries(g.tentativas);
        }

        PDU enviar(PDU pdu) throws ErroSnmp {
            ResponseEvent<UdpAddress> ev;
            try {
                ev = snmp.send(pdu, alvo);
            } catch (IOException e) {
                throw new ErroSnmp("erro de rede: " + e.getMessage());
            }
            PDU resp = ev.getResponse();
            if (resp == null) {
                throw new ErroSnmp("sem resposta (timeout). Confira IP, cabo, se o SNMP está "
                        + "habilitado no switch e a community.");
            }
            if (resp.getErrorStatus() != PDU.noError) {
                String dica = "";
                if (resp.getErrorStatus() == PDU.noAccess || resp.getErrorStatus() == PDU.authorizationError
                        || resp.getErrorStatus() == PDU.notWritable || resp.getErrorStatus() == PDU.readOnly) {
                    dica = " (a community tem permissão de escrita?)";
                } else if (resp.getErrorStatus() == PDU.noCreation || resp.getErrorStatus() == PDU.noSuchName) {
                    dica = " (esse ifIndex existe? rode o comando status)";
                }
                throw new ErroSnmp("agente respondeu erro: " + resp.getErrorStatusText() + dica);
            }
            return resp;
        }

        @Override
        public void setAdminStatus(int ifIndex, int valor) throws ErroSnmp {
            PDU pdu = new PDU();
            pdu.setType(PDU.SET);
            pdu.add(new VariableBinding(new OID(OID_IF_ADMIN + "." + ifIndex), new Integer32(valor)));
            enviar(pdu);
        }

        @Override
        public Map<String, Object> consultar(int ifIndex) throws ErroSnmp {
            PDU pdu = new PDU();
            pdu.setType(PDU.GET);
            pdu.add(new VariableBinding(new OID(OID_IF_DESCR + "." + ifIndex)));
            pdu.add(new VariableBinding(new OID(OID_IF_ADMIN + "." + ifIndex)));
            pdu.add(new VariableBinding(new OID(OID_IF_OPER + "." + ifIndex)));
            PDU resp = enviar(pdu);

            Variable descr = resp.get(0).getVariable();
            Variable admin = resp.get(1).getVariable();
            Variable oper = resp.get(2).getVariable();
            if (admin.isException()) {
                throw new ErroSnmp("ifIndex " + ifIndex + " não existe neste switch (rode o comando status)");
            }
            Map<String, Object> m = new LinkedHashMap<>();
            m.put("if_index", ifIndex);
            m.put("descricao", descr.isException() ? null : descr.toString());
            m.put("admin", nomeStatus(admin.toInt()));
            m.put("oper", oper.isException() ? null : nomeStatus(oper.toInt()));
            return m;
        }

        List<TableEvent> tabela(String... colunas) throws ErroSnmp {
            OID[] oids = new OID[colunas.length];
            for (int i = 0; i < colunas.length; i++) oids[i] = new OID(colunas[i]);
            TableUtils tu = new TableUtils(snmp, new DefaultPDUFactory(PDU.GETNEXT));
            List<TableEvent> linhas = tu.getTable(alvo, oids, null, null);
            if (linhas.size() == 1 && linhas.get(0).isError()) {
                TableEvent e = linhas.get(0);
                String msg = e.getStatus() == TableEvent.STATUS_TIMEOUT
                        ? "sem resposta (timeout). Confira IP, cabo, SNMP habilitado e community."
                        : e.getErrorMessage();
                throw new ErroSnmp(msg);
            }
            List<TableEvent> ok = new ArrayList<>();
            for (TableEvent e : linhas) {
                if (!e.isError() && e.getColumns() != null) ok.add(e);
            }
            return ok;
        }

        @Override
        public List<Map<String, Object>> status() throws ErroSnmp {
            List<Map<String, Object>> portas = new ArrayList<>();
            for (TableEvent e : tabela(OID_IF_DESCR, OID_IF_TYPE, OID_IF_ADMIN, OID_IF_OPER)) {
                VariableBinding[] c = e.getColumns();
                Map<String, Object> m = new LinkedHashMap<>();
                m.put("if_index", e.getIndex().last());
                m.put("descricao", c[0] == null ? null : c[0].getVariable().toString());
                m.put("tipo", c[1] == null ? null : c[1].getVariable().toInt());
                m.put("admin", c[2] == null ? null : nomeStatus(c[2].getVariable().toInt()));
                m.put("oper", c[3] == null ? null : nomeStatus(c[3].getVariable().toInt()));
                portas.add(m);
            }
            return portas;
        }

        @Override
        public List<Map<String, Object>> macs() throws ErroSnmp {
            // porta da bridge -> ifIndex (nem sempre são iguais!)
            Map<Integer, Integer> bridgeParaIf = new TreeMap<>();
            for (TableEvent e : tabela(OID_BASE_IFIDX)) {
                VariableBinding c = e.getColumns()[0];
                if (c != null) bridgeParaIf.put(e.getIndex().last(), c.getVariable().toInt());
            }

            List<Map<String, Object>> lista = new ArrayList<>();
            // 1ª tentativa: BRIDGE-MIB clássica
            for (TableEvent e : tabela(OID_FDB_ADDR, OID_FDB_PORT)) {
                VariableBinding[] c = e.getColumns();
                if (c[0] == null || c[1] == null) continue;
                String mac = ((OctetString) c[0].getVariable()).toHexString(':').toLowerCase(Locale.ROOT);
                lista.add(itemMac(mac, null, c[1].getVariable().toInt(), bridgeParaIf));
            }
            // 2ª tentativa: switches com VLAN costumam usar só a Q-BRIDGE-MIB
            if (lista.isEmpty()) {
                for (TableEvent e : tabela(OID_QFDB_PORT)) {
                    VariableBinding c = e.getColumns()[0];
                    OID idx = e.getIndex(); // vlan.m1.m2.m3.m4.m5.m6
                    if (c == null || idx.size() < 7) continue;
                    StringBuilder mac = new StringBuilder();
                    for (int i = idx.size() - 6; i < idx.size(); i++) {
                        if (mac.length() > 0) mac.append(':');
                        mac.append(String.format("%02x", idx.get(i)));
                    }
                    lista.add(itemMac(mac.toString(), idx.get(0), c.getVariable().toInt(), bridgeParaIf));
                }
            }
            return lista;
        }

        static Map<String, Object> itemMac(String mac, Integer vlan, int portaBridge,
                                           Map<Integer, Integer> bridgeParaIf) {
            Map<String, Object> m = new LinkedHashMap<>();
            m.put("mac", mac);
            if (vlan != null) m.put("vlan", vlan);
            m.put("porta_bridge", portaBridge);
            m.put("if_index", bridgeParaIf.getOrDefault(portaBridge, portaBridge));
            return m;
        }

        @Override
        public void fechar() {
            try { snmp.close(); } catch (IOException ignored) { }
        }
    }

    // ---- Switch simulado (para desenvolver o PHP sem o switch) ------------
    static class SwitchSimulado implements Switch {
        static final int PORTAS = 24;
        final String ip;
        final Path arquivo;
        final Properties estado = new Properties();

        SwitchSimulado(String ip) throws ErroSnmp {
            this.ip = ip;
            String usuario = System.getProperty("user.name", "usuario");
            String caminho = System.getenv("PORTWARDEN_SIM_ARQ");
            arquivo = caminho != null ? Paths.get(caminho)
                    : Paths.get(System.getProperty("java.io.tmpdir"), "port-warden-sim-" + usuario + ".properties");
            if (Files.exists(arquivo)) {
                try (InputStream in = Files.newInputStream(arquivo)) {
                    estado.load(in);
                } catch (IOException e) {
                    throw new ErroSnmp("simulador: não consegui ler " + arquivo + ": " + e.getMessage());
                }
            }
        }

        String chave(int ifIndex) { return ip + "." + ifIndex; }

        void validar(int ifIndex) throws ErroSnmp {
            if (ifIndex < 1 || ifIndex > PORTAS) {
                throw new ErroSnmp("ifIndex " + ifIndex + " não existe neste switch (simulado: 1 a " + PORTAS + ")");
            }
        }

        int admin(int ifIndex) {
            return Integer.parseInt(estado.getProperty(chave(ifIndex), String.valueOf(ADMIN_UP)));
        }

        @Override
        public void setAdminStatus(int ifIndex, int valor) throws ErroSnmp {
            validar(ifIndex);
            estado.setProperty(chave(ifIndex), String.valueOf(valor));
            try (OutputStream out = Files.newOutputStream(arquivo)) {
                estado.store(out, "port-warden: estado do switch simulado");
            } catch (IOException e) {
                throw new ErroSnmp("simulador: não consegui gravar " + arquivo + ": " + e.getMessage());
            }
        }

        @Override
        public Map<String, Object> consultar(int ifIndex) throws ErroSnmp {
            validar(ifIndex);
            int a = admin(ifIndex);
            Map<String, Object> m = new LinkedHashMap<>();
            m.put("if_index", ifIndex);
            m.put("descricao", "Porta " + ifIndex + " (simulada)");
            m.put("admin", nomeStatus(a));
            m.put("oper", a == ADMIN_UP ? "up" : "down");
            return m;
        }

        @Override
        public List<Map<String, Object>> status() throws ErroSnmp {
            List<Map<String, Object>> portas = new ArrayList<>();
            for (int i = 1; i <= PORTAS; i++) {
                Map<String, Object> m = consultar(i);
                m.put("tipo", 6);
                portas.add(m);
            }
            return portas;
        }

        @Override
        public List<Map<String, Object>> macs() {
            List<Map<String, Object>> lista = new ArrayList<>();
            for (int i = 1; i <= 8; i++) {
                Map<String, Object> m = new LinkedHashMap<>();
                m.put("mac", String.format("aa:bb:cc:00:00:%02x", i));
                m.put("porta_bridge", i);
                m.put("if_index", i);
                lista.add(m);
            }
            return lista;
        }

        @Override
        public void fechar() { }
    }

    // =======================================================================
    // Utilitários: JSON mínimo e log
    // =======================================================================
    static Map<String, Object> json(Object... chavesValores) {
        Map<String, Object> m = new LinkedHashMap<>();
        for (int i = 0; i + 1 < chavesValores.length; i += 2) {
            m.put((String) chavesValores[i], chavesValores[i + 1]);
        }
        return m;
    }

    static void imprimir(Map<String, Object> m) { System.out.println(toJson(m)); }
    static void imprimir(String s) { System.out.println(s); }

    @SuppressWarnings("unchecked")
    static String toJson(Object o) {
        if (o == null) return "null";
        if (o instanceof Boolean || o instanceof Number) return o.toString();
        if (o instanceof Map) {
            StringBuilder sb = new StringBuilder("{");
            boolean primeiro = true;
            for (Map.Entry<String, Object> e : ((Map<String, Object>) o).entrySet()) {
                if (!primeiro) sb.append(',');
                primeiro = false;
                sb.append(toJson(e.getKey())).append(':').append(toJson(e.getValue()));
            }
            return sb.append('}').toString();
        }
        if (o instanceof List) {
            StringBuilder sb = new StringBuilder("[");
            boolean primeiro = true;
            for (Object x : (List<Object>) o) {
                if (!primeiro) sb.append(',');
                primeiro = false;
                sb.append(toJson(x));
            }
            return sb.append(']').toString();
        }
        String s = o.toString();
        StringBuilder sb = new StringBuilder("\"");
        for (char ch : s.toCharArray()) {
            switch (ch) {
                case '"':  sb.append("\\\""); break;
                case '\\': sb.append("\\\\"); break;
                case '\n': sb.append("\\n"); break;
                case '\r': sb.append("\\r"); break;
                case '\t': sb.append("\\t"); break;
                default:
                    if (ch < 0x20 || ch > 0x7e) sb.append(String.format("\\u%04x", (int) ch)); // acentos viram escape unicode (seguro sob qualquer locale)
                    else sb.append(ch);
            }
        }
        return sb.append('"').toString();
    }

    static void log(String msg) {
        String agora = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
        System.err.println("[" + agora + "] gerente: " + msg);
    }

    static class ErroUso extends Exception {
        ErroUso(String m) { super(m); }
    }

    static class ErroSnmp extends Exception {
        ErroSnmp(String m) { super(m); }
    }
}
