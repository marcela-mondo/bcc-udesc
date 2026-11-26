import System.IO
import Data.List
import Data.Char

type Doc = String
type Linha = String
type Palavra = String

main = do putStrLn "Nome do arquivo:"
          r <- getLine
          file <- readFile r
          print (file)
          putStrLn "\n\n"
          let resultado = separaLinhas file 
              resultado2 = numLinhas resultado
              resultado3 = limpa (numeraPalavras resultado2)
              resultado4 = ordenar resultado3
              resultado5 = agrupar resultado4
              resultado6 = eliminarRep resultado5
          print (resultado6)

-- monadda IO sem açucar sintátio do
main' = putStrLn "Nome do arquivo:" >> getLine >>= readFile >>= print

limpa [] = []
limpa ((a,b):xs) = if length limpo > 2 then  (a, limpo) :limpa xs else limpa xs
    where limpo = filter isAlphaNum b

separaLinhas :: Doc -> [Linha]
separaLinhas n = lines n

numLinhas :: [Linha] -> [(Int, Linha)]
numLinhas ls = zip [1..] ls

numeraPalavras :: [(Int, Linha)] -> [(Int, Palavra)]
numeraPalavras [] = []
numeraPalavras ((a,b): xs) = zip [a,a..] (words b) ++ numeraPalavras xs

ordenar :: [(Int, Palavra)] -> [(Int, Palavra)]
ordenar [] = []
ordenar ls = inversoDupla (sort (inversoDupla ls))

inversoDupla [] = []
inversoDupla ((a,b):xs) = (b,a) : inversoDupla xs

agrupar :: [(Int, Palavra)] -> [([Int], Palavra)]
agrupar [] = []
agrupar ((a,b):xs) = (a : linhass, b) : agrupar (drop (length linhass) xs)
   where linhass = aux b xs
aux b [] = []
aux b ((c,d):xs) = if b == d then c : aux b xs else aux b xs

eliminarRep :: [([Int], Palavra)] -> [([Int], Palavra)]
eliminarRep [] = []
eliminarRep (((y:ys),b):xs) = (aux' y ys, b) : eliminarRep xs

aux' b [] = [b]
aux' b (x:xs) = if b == x then aux' b xs else b : aux' x xs

