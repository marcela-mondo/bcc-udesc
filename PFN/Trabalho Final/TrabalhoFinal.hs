-- TRABALHO FINAL PFN
-- MARCELA V S S MONDO
import System.IO
import Data.List
import Data.Char

type Doc = String
type Linha = String
type Palavra = String
data Arvore = No Palavra [Int] Arvore Arvore | Folha deriving Show

main = do putStrLn "Nome do arquivo:"
          r <- getLine
          file <- readFile r
          print (file)
          putStrLn "\n\n"
          let resultado = separaLinhas file 
              resultado2 = numLinhas resultado
              resultado3 = limpa (numeraPalavras resultado2)
              resultado4 = mIndextree resultado3
          print (resultado4)

-- Limpagem do texto
limpa [] = []
limpa ((a,b):xs) = if length limpo > 2 then  (a, limpo) :limpa xs else limpa xs
    where limpo = filter isAlphaNum b

-- A) SEPARA O DOCUMENTO EM LINHAS
separaLinhas :: Doc -> [Linha]
separaLinhas n = lines n

-- B) NUMERA AS LINHAS DO DOCUMENTO
numLinhas :: [Linha] -> [(Int, Linha)]
numLinhas ls = zip [1..] ls

-- C) ASSOCIA A CADA OCORRÊNCIA DE UMA PALAVRA, O NÚMERO DA LINHA QUE ELA OCORRE
numeraPalavras :: [(Int, Linha)] -> [(Int, Palavra)]
numeraPalavras [] = []
numeraPalavras ((a,b): xs) = zip [a,a..] (words b) ++ numeraPalavras xs

-- D) INSERE ELEMENTOS EM UMA LISTA ORDENADA
insOrd ::(Ord a) => a -> [a] -> [a]
insOrd n [] = [n]
insOrd n (x:xs) | n == x = x : xs
                | n < x = n : x : xs
                | otherwise = x : insOrd n (xs) 

-- E) INSERE UMA PALAVRA E LINHA DE OCORRÊNCIA NA ÁRVORE
ins :: Palavra -> Int -> Arvore -> Arvore
ins a n Folha = No a [n] Folha Folha
ins a n (No x ls esq dir) | a < x = No x ls (ins a n esq) dir
                          | a > x = No x ls esq (ins a n dir)
                          | a == x = No x (insOrd n ls) esq dir

-- F) PERCORRE A LISTA COM AS DUPLAS DE PALAVRAS E LINHAS INSERIDO CADA UMA DELAS NA ÁRVORE
mIndextree :: [(Int, Palavra)] -> Arvore
mIndextree ls = mIndextree' Folha ls 

mIndextree' arv [] = arv
mIndextree' arv ((a,b):xs) = mIndextree' (ins b a arv) xs