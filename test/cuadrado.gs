
var distancia = 100
var vueltas = 4

penColor("blue")
penDown()

if (distancia > 50) {
    repeat (vueltas) {
        forward(distancia)
        rotate(90)
    }
}
