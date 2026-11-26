-- LISTA 1 MARCELA VITÓRIA DA SILVA SAVI MONDO
-- Questão 1 
ehTriangulo x y z = (x + y > z) && (x + z > y) && (y + z > x)

-- Questão 2 
tipoTriangulo x y z | ( (x==y) && (y==z)) = "Equilatero!"
                    | ( (x==y) || (x==z) || (y==z)) = "Isosceles!"
                    | otherwise = "Escaleno!"

-- Questão 3 
triangulo x y z | (ehTriangulo x y z) = tipoTriangulo x y z
                | otherwise = "Nao eh triangulo"


-- Questão 4 
somaPares 0 = 0
somaPares n = (if mod n 2 == 0 then n + somaPares(n-1) else 0 + somaPares(n-1)) 

-- Questão 5
somaPot m n = somaPotaux m n 0
somaPotaux m n p = if p <= n then ((2^p)*m) + somaPotaux m n (p+1) else 0

-- Questao 6
primo 1 = False
primo 0 = False
primo n = primoaux n 2
primoaux n p | (p == n)= True
             | (mod n p == 0) = False
             | otherwise = primoaux n (p+1)


-- Questao 7
seriepi n = if mod n 2 == 0 then seriepiaux (n-1) 1 1 else seriepiaux (n-2) 1 1

seriepiaux n p s | n == (round p) = s*4/p
                 | otherwise = s*(4/p) + seriepiaux n (p+2) (-s)