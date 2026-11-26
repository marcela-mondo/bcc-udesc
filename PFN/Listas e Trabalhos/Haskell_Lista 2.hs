-- LISTA 2 --
-- MARCELA VITÓRIA DA SILVA SAVI MONDO --
import Data.Char

-- Questão 1 -- 
pertence n [] = False 
pertence n (x:xs) = if n == x then True else pertence n xs

-- Questão 2 --
intersecao _ [] = []
intersecao [] _ = []
intersecao (x:xs) (z:zs) | pertence x (z:zs) = x : intersecao xs (z:zs)
                         | otherwise = intersecao xs (z:zs)

-- Questão 3 --
inverso [] = []
inverso (x:xs) = inverso (xs) ++ [x]

-- Questão 4 --
nUltimos m ls = nUltimos' (length ls - m) ls

nUltimos' 0 xs = xs
nUltimos' n [] = []
nUltimos' n (x:xs) | n > 0 = nUltimos' (n-1) xs
                   | otherwise = (x:xs)

-- Questão 5 --
soma2 [] _ = []
soma2 _ [] = []
soma2 (x:xs) (z:zs) = (x+z) : soma2 xs zs

-- Questão 6 --
pot2 1 = [2]
pot2 n = pot2 (n-1) ++ [2^n]

-- Questão 7 --
intercalacao [] (z:zs) = (z:zs)
intercalacao (x:xs) [] = (x:xs)
intercalacao [] [] = []
intercalacao (x:xs) (z:zs) = if (x < z) then (x: intercalacao (xs) (z:zs)) else (z : intercalacao (zs) (x:xs)) 

-- Questão 8 --
menor [n] = n
menor (x:xs) = if (x < menor xs) then (x) else menor (xs)

-- Questão 9 --
removerElem n [] = []
removerElem n (x:xs) = if n == x then (xs) else x:removerElem n (xs) 

-- Questão 10 --
ordenar [] = []
ordenar ls = menor ls: ordenar (removerElem (menor ls) ls)

-- Questão 11 --
insereOrd n [] = [n]
insereOrd n (x:xs) | n == x = x : xs
                   | n < x = n : x : xs
                   | otherwise = x : insereOrd n (xs) 

-- Questão 12 --
enesimo 0 (x:xs) = error "Não encontrado"
enesimo n [] = error "Não encontrado"
enesimo 1 (x:xs) = (x)
enesimo n (x:xs) = enesimo (n-1) (xs)

-- Questão 13 --
repetir 0 e = []
repetir n e = e : repetir (n-1) (e)

-- Questão 14 --
removeTab [] = []
removeTab (x:xs) = if x == '\t' then ' ' : removeTab (xs) else (x) : removeTab (xs)

-- Questão 15 --
minusculas [] = []
minusculas (x:xs) = toLower (x) : minusculas (xs)

-- Questão 16 --
inversoDupla [] = []
inversoDupla ((a,b):xs) = ((b,a) : inversoDupla (xs))

-- Questão 17 --
simetrico [] = []
simetrico ((a,b):xs) = if (a == b) then True : simetrico (xs) else False : simetrico (xs)

-- Questão 18 --
numString 0 = []
numString n = numString (div n 10) ++ [(chr ((mod n 10) + 48))]

-- Questão 19 --
stringNum [] = 0
stringNum (x:xs) = (ord x - 48)  * (10^(length(x:xs)-1)) + stringNum xs

-- Questão 20 --
decBin' 0 = []
decBin' n | mod n 2 == 1 = '1' : decBin' (div n 2)
          | mod n 2 == 0 = '0' : decBin' (div n 2)

decBin n = inverso (decBin' n)

-- Questão 21 --
binDec [] = 0 
binDec (x:xs) = ((ord x - 48))*2^(length xs) + binDec xs

-- Questão 22 --
trocoCafe n m = trocoCafe' (m - n) [50,20,10,5]
trocoCafe' n [] = []
trocoCafe' n (x:xs) = if (div n x > 0) then (x, div n x) : trocoCafe' (mod n x) xs else trocoCafe' n xs 