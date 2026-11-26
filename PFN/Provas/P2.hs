-- Aluna: Marcela Vitória da Silva Savi Mondo

-- EXERCÍCIO 1
--(λx.λy.(x(λu.Vv.u))y)(λa.λb.a)(λc.Vd.d)
--(λy.((λa.λb.a)(λu.λv.u))y)(λc.λd.d)
--((λa.λb.a)(λu.λv.u))(λc.Vd.d)
--(λu.λv.u)(λc.d.d)
--(λc.λd.d)

-- EXERCÍCIO 2 
compDuplas [] = []
compDuplas ((a,b):xs) = ((b,a) : compDuplas (xs))
-- Na primeira chamada da função, a posição do primeiro elemento da lista é trocada com o do segundo elemento, e usando o construtor ":" temos que a calda dessa lista, será a recursão
-- da função compDuplas, passando a calda da lista que foi passada no início. Essa calda então retornará as novas cabeças da lista, para então repetir todo o processo. 
-- ex: compDuplas[(1, 2), (6, 1), (4, 11)] => [(2, 1), (1, 6), (11, 4)]

-- EXERCÍCIO 3
zip' :: (a -> b -> c) -> [a] -> [b] -> [c]  
zip' _ [] _ = []  
zip' _ _ [] = []  
zip' f (x:xs) (y:ys) = f x y : zip' f xs ys 
-- A função recebe como primeiro parâmetro o recebimento de dois elementos e produz um terceiro. O segundo e o terceiro parâmetro sâo listas, assim como o resultado também é.
-- O primeiro tem que ser uma lista de as, assim como a segunda tem que ser uma lista de bs, resultando em uma lista de cs. 
-- ex: zip' [4,2,5,6] [2,6,2,3] => [6,8,7,9] 

-- EXERCÍCIO 4
semVogais :: [String] -> [String]
semVogais ((a,b):xs) | elem x "aeiouAEIOU" = semVogais xs
                     | otherwise = x : (semVogais xs)
-- Recebendo uma lista de strings, o código verifica se o elemento da cabeça da lista é igual aos caracteres da string "aeiouAEIOU", caso seja, ele "ignora" esse elemento e chama a recursão da lista passando a calda.
-- Case não seja igual, cai no otherwise, mantendo a cabeça da lista, e para manter tudo isso ainda dentro de uma lista, usamos o construtor ":", chamando então a recursão da função, chamando a calda da lista.
-- ex: semVogal ["The", "end"] => ["th", "nd"]

-- EXERCÍCIO 5
data Arvore a = Folha | Galho a (Arvore a) (Arvore a) devering show
maiorArv :: (a -> b) -> Arvore a -> Arvore b
maiorArv f Folha = Folha
maiorArv f (Galho x a1 a2) = (Galho (f x) (maiorArv f a1) (maiorArv f a2))
-- A função compara o elemento que recebe com o primeiro nó, caso o elemento seja maior, é depositado no nó da direita, caso menor, no nó da esquerda. O programa verifica qual nó está mais a direita e comparando entre si para ver qual o maior, ele chega no resultado quando encontrar a condição de parada que é apenas as folhas vazias, assim é retornado o resultado.
-- ex: maiorArv (No 6(No 3 Folha Folha) (No 10 (Folha (No 15 Folha Folha)))) => 15