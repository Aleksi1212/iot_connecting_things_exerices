import express from 'express';
import path from 'path';

const app = express()
const port = 3000

const min = 1
const max = 10

app.get('/', (req, res) => {
    res.sendFile(path.resolve("./index.html"))
})

app.get("/:name", (req, res) => {
    const randNum =  Math.floor(Math.random() * (max - min + 1)) + min;
    const { num } = req.query
    const { name } = req.params

    if (!num) {
        return res.redirect('/')
    }
    if (num > max || num < min) {
        return res.send(
            `Hey ${name}!!! You need to read the instructions!!! The number im thinking of is between ${min}-${max}!!!`
        )
    }

    if (num == randNum) {
        res.send(`Yaaaay great job ${name}! You chose the correct value of ${randNum}!`)
    } else{
        res.send(`Uh-oh ${name} the number I was thinking of was ${randNum} not ${num} :(`)
    }
})

app.listen(port, () => {
    console.log(`Listening on port: ${port}`)
});