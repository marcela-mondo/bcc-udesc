Algoritmo "Farmácia"

Var
   idade, escolher_tipo_produto, escolher_recomprar, escolher_menu_medicamentos, escolher_menu_higiene: inteiro
   contador_aspirina, contador_eno, contador_escova, contador_fiodental: inteiro
   preco_aspirina, preco_eno, preco_escova, preco_fiodental, total: real
   nome: caracter

Inicio

preco_aspirina <- 8.99
preco_eno <- 22.79
preco_escova <- 25.49
preco_fiodental <- 10.89

escreval("REALIZE SEU CADASTRO")
escreva("Digite seu nome: ")
leia(nome)
escreva("Digite sua idade: ")
leia(idade)


   repita
      escreval("-----------------------------------")
      escreval(nome, " BEM VINDO(A) A FARMÁCIA RENEGADOS")
      escreval("-----------------------------------")
      escreval("")
      escreval("ESCOLHA O TIPO DE PRODUTO")
      escreval("1. MEDICAMENTOS")
      escreval("2. PRODUTOS DE HIGIENE")
      escreval("3. PARAR DE COMPRAR")
      escreval("")
      escreva("Digite aqui: ")
      leia(escolher_tipo_produto)
      escolha escolher_tipo_produto
      caso 1
         se idade >= 18 entao
            repita
               escreval(" ")
               escreval("MEDICAMENTOS")
               escreval("01. Aspirina")
               escreval("02. Sal de Fruta Eno Tônica")
               escreval("03. Voltar")
               escreval("")
               escreva("Digite aqui: ")
               leia(escolher_menu_medicamentos)
               escolha escolher_menu_medicamentos
               
               caso 1
                  escreval("Nome comercial: aspirina")
                  escreval("Preço: R$", preco_aspirina)
                  escreval("Tipo produto: medicamento")
                  escreval("Princípio (substância): ácido acetilsalicílico")
                  escreval("Forma de administração: via oral")
                  escreval("Tipo Embalagem: caixa")
                  escreval("Quantidade: 100 comprimidos")
                  escreval("Apresentação: comprimidos ")
                  escreval("Concentração: 100mg")
                  escreval("Fabricante/Laboratório: EMS")
                  escreval("Tipo: genérico")
                  escreval("Uso: pedriátrico")
                  escreval("Descrição: sabor morango, infantil")
                  escreval("Classe de remédio: analgésico, antitérmico")
                  escreval("")

                  repita
                     escreval("")
                     escreval("01. COMPRAR")
                     escreval("02. VOLTAR")
                     escreva("Digite aqui: ")
                     leia(escolher_recomprar)
                     SE (escolher_recomprar = 1) ENTAO
                        contador_aspirina <- contador_aspirina + 1
                        escreval("Você comprou ", contador_aspirina, " unidade(s)")
                     fimse
                  ate escolher_recomprar = 2

               caso 2
                  escreval("Nome comercial:Sal de Fruta Eno Tônica")
                  escreval("Preço: R$", preco_eno)
                  escreval("Tipo Produto: medicamento")
                  escreval("Princípio: bicarbonato de sodio, carbonato de sódio, ácido citrico")
                  escreval("Forma de administração: via oral")
                  escreval("Tipo Embalagem: Frasco")
                  escreval("Quantidade: 100g")
                  escreval("Apresentação: pó enfervescente")
                  escreval("Concentração: Diluir em um copo de água de 300ml")
                  escreval("Tipo: genérico")
                  escreval("Uso: adulto, pediátrico")
                  escreval("Descrição: 20 doses")
                  escreval("Classe de remédio: antiácido")
                  escreval("")
                  
                  repita
                     escreval("")
                     escreval("01. COMPRAR")
                     escreval("02. VOLTAR")
                     escreva("Digite aqui: ")
                     leia(escolher_recomprar)
                     SE (escolher_recomprar = 1) ENTAO
                        contador_eno <- contador_eno + 1
                        escreval("Você comprou ", contador_eno, " unidade(s)")
                     fimse
                  ate escolher_recomprar = 2
                  
               fimescolha
            ate escolher_menu_medicamentos = 3
         senao
              escreval("")
              escreval("Não autorizado")
         fimse
         

      caso 2
            repita
               escreval("PRODUTOS DE HIGIENE")
               escreval("01. Escova de Dente Colgate Slim Soft 4 unidades")
               escreval("02. Fio Dental Colgate Total 50m ")
               escreval("03. Voltar")
               escreval("")
               escreva("Digite aqui: ")
               leia(escolher_menu_higiene)
               escolha escolher_menu_higiene

               caso 1
                  escreval(" ")
                  escreval("Tipo Produto: Higiente pessoal")
                  escreval("Preço: R$", preco_escova)
                  escreval("Nome comercial: Escova de Dente Colgate Slim Soft 4 unidades")
                  escreval("Tipo Embalagem: Embalagem plastificada")
                  escreval("Quantidade: 4 unidades")
                  escreval("Fabricante/Laboratório: Colgate")
                  escreval("Tipo: genérico")
                  escreval("Uso: adulto, pediátrico")
                  escreval("Descrição: Macia")
                  escreval("")
                  
                  repita
                     escreval("")
                     escreval("01. COMPRAR")
                     escreval("02. VOLTAR")
                     escreva("Digite aqui: ")
                     leia(escolher_recomprar)
                     SE (escolher_recomprar = 1) ENTAO
                        contador_escova <- contador_escova + 1
                        escreval("Você comprou ", contador_escova, " unidade(s)")
                     fimse
                  ate escolher_recomprar = 2

               caso 2
                  escreval("Tipo Produto: Higiente pessoal")
                  escreval("Preço: R$", preco_fiodental)
                  escreval("Nome comercial: Fio Dental Colgate Total 50m")
                  escreval("Tipo Embalagem: Embalagem plastificada")
                  escreval("Quantidade: 50 m")
                  escreval("Fabricante/Laboratório: Colgate")
                  escreval("Tipo: genérico")
                  escreval("Uso: adulto, pediátrico")
                  escreval("Descrição: Fio encerado")
                  escreval("")
                  
                  repita
                     escreval("")
                     escreval("01. COMPRAR")
                     escreval("02. VOLTAR")
                     escreva("Digite aqui: ")
                     leia(escolher_recomprar)
                     SE (escolher_recomprar = 1) ENTAO
                        contador_fiodental <- contador_fiodental + 1
                        escreval("Você comprou ", contador_fiodental, " unidade(s)")
                     fimse
                  ate escolher_recomprar = 2
               fimescolha
            ate escolher_menu_higiene = 3
      fimescolha
      
   ate escolher_tipo_produto = 3

   total <- ((contador_aspirina * preco_aspirina) + (contador_eno * preco_eno) + (contador_escova * preco_escova) + (contador_fiodental * preco_fiodental))
   escreval(" ")
   SE (contador_aspirina = 0) E (contador_eno = 0) E (contador_escova = 0) E (contador_fiodental = 0) ENTAO
      escreval("OBRIGADO POR VISITAR A FARMÁCIA RENEGADOS, VOLTE SEMPRE!")
   senao
        escreval("CARRINHO DE COMPRAS")
        SE (contador_aspirina >= 1) ENTAO
           escreval("R$", (contador_aspirina * preco_aspirina), " X", contador_aspirina, " Aspirina")
        FIMSE
   
        SE (contador_eno >= 1) ENTAO
           escreval("R$", (contador_eno * preco_eno), " X", contador_eno, " Sal de Fruta Eno Tônica")
           FIMSE
   
        SE (contador_escova >= 1) ENTAO
      escreval("R$", (contador_escova * preco_escova), " X", contador_escova, " Escova de Dente Colgate Slim Soft 4 unidades")
        FIMSE
   
        SE (contador_fiodental >= 1) ENTAO
           escreval("R$", (contador_fiodental * preco_fiodental), " X", contador_fiodental, " Fio Dental Colgate Total 50m")
        FIMSE
        escreval("-----------------------------------")
        escreval("TOTAL A SER PAGO R$", total)
   fimse

   
Fimalgoritmo