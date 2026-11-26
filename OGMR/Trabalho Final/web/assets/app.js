/*
 * port-warden | assets/app.js
 * Desenha a grade de portas e a tabela de agendamentos a partir de
 * api/status.php, que é consultada sozinha a cada poucos segundos.
 * Todo texto vindo do servidor entra via textContent (sem risco de XSS).
 */
(() => {
  'use strict';

  const app = document.getElementById('app');
  if (!app) return;

  const INTERVALO = Number(app.dataset.intervalo) || 5000;
  const CSRF = document.querySelector('meta[name="csrf-token"]')?.content || '';
  const $ = (id) => document.getElementById(id);

  let portas = [];
  let agendamentos = [];
  const selecionadas = new Set();
  let timer = null;
  let ocupado = false;

  // -------------------------------------------------------------------------
  // Utilitários
  // -------------------------------------------------------------------------
  function el(tag, props = {}, ...filhos) {
    const e = document.createElement(tag);
    for (const [k, v] of Object.entries(props)) {
      if (v === null || v === undefined || v === false) continue;
      if (k === 'class') e.className = v;
      else if (k === 'text') e.textContent = v;
      else if (k.startsWith('on')) e.addEventListener(k.slice(2), v);
      else e.setAttribute(k, v === true ? '' : v);
    }
    for (const f of filhos.flat()) {
      if (f === null || f === undefined || f === false) continue;
      e.append(f instanceof Node ? f : document.createTextNode(String(f)));
    }
    return e;
  }

  async function api(url, dados) {
    const opcoes = { credentials: 'same-origin', headers: {} };
    if (dados !== undefined) {
      opcoes.method = 'POST';
      opcoes.headers['Content-Type'] = 'application/json';
      opcoes.headers['X-CSRF-Token'] = CSRF;
      opcoes.body = JSON.stringify(dados);
    }
    const r = await fetch(url, opcoes);
    if (r.status === 401) {
      location.href = 'login.php';
      throw new Error('Sessão expirada.');
    }
    let j;
    try { j = await r.json(); } catch { throw new Error(`Resposta inválida do servidor (HTTP ${r.status}).`); }
    if (r.status === 403 && dados === undefined) location.reload();
    return j;
  }

  /** '2026-11-24 14:30:00' -> '14:30' (hoje) ou '24/11 14:30' */
  function hora(s) {
    if (!s) return '';
    const [d, t] = s.split(' ');
    const [, m, dia] = d.split('-');
    const hoje = new Date();
    const ehHoje = Number(dia) === hoje.getDate() && Number(m) === hoje.getMonth() + 1;
    return (ehHoje ? '' : `${dia}/${m} `) + t.slice(0, 5);
  }

  function alerta(tipo, titulo, detalhes = []) {
    const caixa = el('div', { class: `alert alert-${tipo} shadow-sm small mb-2`, role: 'alert' },
      el('button', { type: 'button', class: 'btn-close float-end', 'aria-label': 'Fechar',
                     onclick: (ev) => ev.target.closest('.alert').remove() }),
      el('strong', { text: titulo }),
      detalhes.length ? el('ul', { class: 'mb-0 mt-1 ps-3' }, detalhes.map((d) => el('li', { text: d }))) : null);
    $('alertas').prepend(caixa);
    setTimeout(() => caixa.remove(), tipo === 'success' ? 5000 : 12000);
  }

  const ICONE_FECHADO = 'M8 1a2 2 0 0 1 2 2v4H6V3a2 2 0 0 1 2-2m3 6V3a3 3 0 0 0-6 0v4a2 2 0 0 0-2 2v5a2 2 0 0 0 2 2h6a2 2 0 0 0 2-2V9a2 2 0 0 0-2-2';
  const ICONE_ABERTO = 'M11 1a2 2 0 0 0-2 2v4a2 2 0 0 1 2 2v5a2 2 0 0 1-2 2H3a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2h5V3a3 3 0 0 1 6 0v4a.5.5 0 0 1-1 0V3a2 2 0 0 0-2-2';

  function icone(fechado) {
    const ns = 'http://www.w3.org/2000/svg';
    const svg = document.createElementNS(ns, 'svg');
    svg.setAttribute('viewBox', '0 0 16 16');
    svg.setAttribute('width', '20');
    svg.setAttribute('height', '20');
    svg.setAttribute('fill', 'currentColor');
    svg.setAttribute('aria-hidden', 'true');
    const p = document.createElementNS(ns, 'path');
    p.setAttribute('d', fechado ? ICONE_FECHADO : ICONE_ABERTO);
    svg.append(p);
    return svg;
  }

  function estado(p) {
    if (p.com_defeito) return { classe: 'defeito', rotulo: 'Com defeito', fechado: true };
    if (p.protegida) return { classe: 'protegida', rotulo: 'Protegida', fechado: p.admin === 'down' };
    if (p.admin === 'down') return { classe: 'bloqueada', rotulo: 'Bloqueada', fechado: true };
    if (p.admin === 'up') return { classe: 'liberada', rotulo: 'Liberada', fechado: false };
    return { classe: 'desconhecido', rotulo: 'Sem leitura', fechado: false };
  }

  // -------------------------------------------------------------------------
  // Desenho
  // -------------------------------------------------------------------------
  function filtroBusca(p) {
    const termo = $('busca').value.trim().toLowerCase();
    if (!termo) return true;
    return [p.host, p.ip, p.mac, `porta ${p.porta}`, String(p.porta), p.tipo]
      .some((v) => v && String(v).toLowerCase().includes(termo));
  }

  function desenharGrade() {
    const grade = $('grade');
    grade.replaceChildren();
    const visiveis = portas.filter(filtroBusca);
    if (!visiveis.length) {
      grade.append(el('div', { class: 'col-12 text-secondary', text: 'Nenhuma porta encontrada.' }));
    }
    for (const p of visiveis) {
      const est = estado(p);
      const marcada = selecionadas.has(p.porta_id);
      const detalhes = [];
      if (p.bloqueado_ate) detalhes.push(el('div', { class: 'small fw-semibold', text: `até ${hora(p.bloqueado_ate)}` }));
      if (p.agendado_para) detalhes.push(el('div', { class: 'small text-warning-emphasis', text: `agendado ${hora(p.agendado_para)}` }));
      if (p.admin === 'up' && p.link === 'down' && !p.com_defeito) detalhes.push(el('div', { class: 'small text-secondary', text: 'sem cabo/desligada' }));

      const nome = p.host || (p.tipo === 'livre' ? 'Porta livre' : p.tipo);
      const card = el('div', {
        class: `card porta h-100 estado-${est.classe}${marcada ? ' selecionada' : ''}${p.bloqueavel ? ' clicavel' : ''}`,
        title: p.observacao || '',
        onclick: p.bloqueavel ? () => alternar(p.porta_id) : null,
      },
        el('div', { class: 'card-body p-2' },
          el('div', { class: 'd-flex justify-content-between align-items-center mb-1' },
            el('span', { class: 'fw-bold' },
              p.bloqueavel ? el('input', { type: 'checkbox', class: 'form-check-input me-1', checked: marcada,
                                           'aria-label': `Selecionar porta ${p.porta}`, tabindex: '-1' }) : null,
              `Porta ${p.porta}`),
            el('span', { class: 'cadeado' }, icone(est.fechado))),
          el('div', { class: 'small text-truncate fw-semibold', text: nome }),
          p.ip ? el('div', { class: 'small text-secondary mono text-truncate', text: p.ip }) : null,
          p.mac ? el('div', { class: 'small text-secondary mono text-truncate', text: p.mac }) : null,
          el('div', { class: 'mt-1' }, el('span', { class: `badge rotulo-${est.classe}`, text: est.rotulo })),
          detalhes));
      grade.append(el('div', { class: 'col-6 col-md-4 col-lg-3 col-xl-2' }, card));
    }
    atualizarContagem();
  }

  function desenharAgendamentos() {
    const corpo = $('tabela-agendamentos');
    corpo.replaceChildren();
    if (!agendamentos.length) {
      corpo.append(el('tr', {}, el('td', { colspan: '6', class: 'text-secondary small', text: 'Nenhum bloqueio ativo ou agendado.' })));
      return;
    }
    const porLote = {};
    for (const a of agendamentos) porLote[a.lote] = (porLote[a.lote] || 0) + 1;
    const lotesMostrados = new Set();

    for (const a of agendamentos) {
      const ativo = a.status === 'ativo';
      const botoes = [el('button', { class: 'btn btn-sm btn-outline-secondary', text: ativo ? 'Liberar' : 'Cancelar',
                                     onclick: () => cancelar({ id: a.id }) })];
      if (a.lote && porLote[a.lote] > 1 && !lotesMostrados.has(a.lote)) {
        lotesMostrados.add(a.lote);
        botoes.push(el('button', { class: 'btn btn-sm btn-outline-danger ms-1', text: `Cancelar grupo (${porLote[a.lote]})`,
                                   onclick: () => cancelar({ lote: a.lote }, porLote[a.lote]) }));
      }
      corpo.append(el('tr', {},
        el('td', { text: a.porta_numero }),
        el('td', { text: a.host_nome || '—' }),
        el('td', { text: hora(a.inicio) }),
        el('td', { text: hora(a.fim) }),
        el('td', {}, el('span', { class: `badge ${ativo ? 'text-bg-danger' : 'text-bg-warning'}`, text: ativo ? 'Bloqueada' : 'Agendada' })),
        el('td', { class: 'text-end text-nowrap' }, botoes)));
    }
  }

  function atualizarContagem() {
    // descarta seleções de portas que não são mais bloqueáveis
    const validas = new Set(portas.filter((p) => p.bloqueavel).map((p) => p.porta_id));
    for (const id of [...selecionadas]) if (!validas.has(id)) selecionadas.delete(id);
    const n = selecionadas.size;
    $('contagem').textContent = n ? `${n} porta(s) selecionada(s)` : 'Nenhuma porta selecionada';
    $('btn-bloquear').disabled = n === 0;
    $('btn-desbloquear').disabled = n === 0;
  }

  function alternar(id) {
    if (selecionadas.has(id)) selecionadas.delete(id); else selecionadas.add(id);
    desenharGrade();
  }

  // -------------------------------------------------------------------------
  // Atualização automática
  // -------------------------------------------------------------------------
  async function atualizar() {
    clearTimeout(timer);
    $('indicador').classList.add('ativo');
    try {
      const j = await api('api/status.php');
      if (!j.ok) throw new Error(j.erro || 'erro desconhecido');
      portas = j.portas;
      agendamentos = j.agendamentos;
      const bloqueadas = portas.filter((p) => p.admin === 'down' && !p.com_defeito).length;
      const bloqueaveis = portas.filter((p) => p.bloqueavel).length;
      $('resumo').textContent = `${bloqueadas} bloqueada(s) de ${bloqueaveis} porta(s) controláveis`
        + (j.simulado ? ' · MODO SIMULADO' : '');
      $('atualizado').textContent = `atualizado às ${j.agora.slice(11, 19)}`;
      const caixa = $('erro-switch');
      if (j.erros_switch && j.erros_switch.length) {
        caixa.textContent = 'Sem leitura do switch: ' + j.erros_switch.join(' | ');
        caixa.classList.remove('d-none');
      } else {
        caixa.classList.add('d-none');
      }
      desenharGrade();
      desenharAgendamentos();
    } catch (e) {
      $('atualizado').textContent = `falha ao atualizar: ${e.message}`;
    } finally {
      $('indicador').classList.remove('ativo');
      timer = setTimeout(atualizar, INTERVALO);
    }
  }

  // -------------------------------------------------------------------------
  // Ações
  // -------------------------------------------------------------------------
  function periodo() {
    const modo = document.querySelector('input[name="modo"]:checked').value;
    return {
      modo,
      inicio: modo === 'agendar' ? $('inicio').value : null,
      duracao: Number($('duracao').value),
      unidade: $('unidade').value,
    };
  }

  function descreverPeriodo(per) {
    const unidades = { min: 'minuto(s)', h: 'hora(s)', d: 'dia(s)' };
    const quando = per.modo === 'agora' ? 'agora' : `em ${per.inicio.replace('T', ' ')}`;
    return `${quando}, por ${per.duracao} ${unidades[per.unidade]}`;
  }

  function nomePorta(r) {
    const p = portas.find((x) => x.porta_id === r.porta_id);
    return `Porta ${r.porta ?? p?.porta ?? r.porta_id}${p?.host ? ' (' + p.host + ')' : ''}`;
  }

  async function executar(acao, url, dados, mensagemOk) {
    if (ocupado) return;
    ocupado = true;
    document.body.classList.add('ocupado');
    try {
      const j = await api(url, dados);
      if (j.resultados) {
        const falhas = j.resultados.filter((r) => !r.ok);
        if (!falhas.length) {
          alerta('success', mensagemOk(j));
        } else {
          alerta(falhas.length === j.resultados.length ? 'danger' : 'warning',
            `${acao}: ${falhas.length} de ${j.resultados.length} falharam`,
            falhas.map((r) => `${r.porta_id ? nomePorta(r) : 'Agendamento ' + r.id}: ${r.erro}`));
        }
      } else if (!j.ok) {
        alerta('danger', `${acao}: ${j.erro}`);
      }
      selecionadas.clear();
    } catch (e) {
      alerta('danger', `${acao}: ${e.message}`);
    } finally {
      ocupado = false;
      document.body.classList.remove('ocupado');
      atualizar();
    }
  }

  function bloquear(alvo) {
    const per = periodo();
    if (!(per.duracao >= 1)) return alerta('warning', 'Informe por quanto tempo bloquear.');
    if (per.modo === 'agendar' && !per.inicio) return alerta('warning', 'Informe a data e hora de início.');
    const qtd = alvo === 'sala' ? portas.filter((p) => p.bloqueavel).length : alvo.length;
    if (!confirm(`Bloquear ${qtd} porta(s) ${descreverPeriodo(per)}?`)) return;
    executar('Bloquear', 'api/bloquear.php', { portas: alvo, ...per },
      (j) => `${j.total} porta(s) ${j.imediato ? 'bloqueada(s)' : 'agendada(s)'} até ${hora(j.fim)}.`);
  }

  function desbloquear(alvo) {
    const qtd = alvo === 'sala' ? portas.filter((p) => p.bloqueavel).length : alvo.length;
    if (!confirm(`Liberar ${qtd} porta(s) agora? Os agendamentos delas serão cancelados.`)) return;
    executar('Liberar', 'api/desbloquear.php', { portas: alvo }, (j) => `${j.total} porta(s) liberada(s).`);
  }

  function cancelar(dados, qtd = 1) {
    if (!confirm(qtd > 1 ? `Cancelar os ${qtd} bloqueios deste grupo?` : 'Cancelar este bloqueio?')) return;
    executar('Cancelar', 'api/cancelar.php', dados, (j) => `${j.total} bloqueio(s) cancelado(s).`);
  }

  // -------------------------------------------------------------------------
  // Eventos
  // -------------------------------------------------------------------------
  document.querySelectorAll('input[name="modo"]').forEach((r) => r.addEventListener('change', () => {
    $('campo-inicio').hidden = periodo().modo !== 'agendar';
  }));
  $('btn-sala').addEventListener('click', () => bloquear('sala'));
  $('btn-liberar-sala').addEventListener('click', () => desbloquear('sala'));
  $('btn-bloquear').addEventListener('click', () => bloquear([...selecionadas]));
  $('btn-desbloquear').addEventListener('click', () => desbloquear([...selecionadas]));
  $('busca').addEventListener('input', desenharGrade);
  $('sel-todas').addEventListener('click', (e) => {
    e.preventDefault();
    portas.filter((p) => p.bloqueavel && filtroBusca(p)).forEach((p) => selecionadas.add(p.porta_id));
    desenharGrade();
  });
  $('sel-nenhuma').addEventListener('click', (e) => {
    e.preventDefault();
    selecionadas.clear();
    desenharGrade();
  });
  // Ao voltar para a aba, atualiza na hora
  document.addEventListener('visibilitychange', () => { if (!document.hidden) atualizar(); });

  atualizar();
})();
