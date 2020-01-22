const Encrypt = require("./encrypt_linux/encrypt")

let data = "a".repeat(1000000)

Encrypt.writeFile("teste.txt", data, "Teste123", (processed, total) => {
    console.log(processed / total)
})