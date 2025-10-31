const express = require('express');
const mqtt = require('mqtt');
const nanoid = require('nanoid');
const path = require('path');
const util = require('util');
const fs = require('fs');
const bodyParser = require('body-parser');
const app = express();

const PORT = 3000;

const readFile = util.promisify(fs.readFile);
const writeFile = util.promisify(fs.writeFile);
// Create variables for MQTT use here
const mqttBroker = "192.168.50.111"
const mqttPort = 1883

app.use(bodyParser.json());
function read(filePath = './message.json') {
    return readFile(path.resolve(__dirname, filePath)).then((data) =>
        JSON.parse(data)
    );
}
function write(data, filePath = './message.json') {
    return writeFile(path.resolve(__dirname, filePath), JSON.stringify(data));
}

// create an MQTT instance
let client = mqtt.connect(`mqtt://${mqttBroker}:${mqttPort}`)

// Check that you are connected to MQTT and subscribe to a topic (connect event)
client?.on("connect", async () => {
    const topics = await read("./topics.json")
    for (let i = 0; i < topics.length; i++) {
        const { name } = topics[i];
        client.subscribe(name, (err) => {
            if (!err) {
                client.publish(name, `Hello ${name}!`);
                console.log(`Connected to topic: ${name}`)
            } else {
                console.log(`Error connecting to topic: ${name}`)
            }
        })
    }
    // for (let i = 0; i < mqttSendTopic.length; i++) {
    //     const topic = mqttSendTopic[i]
    //     client.subscribe(topic, (err) => {
    //         if (!err) {
    //             client.publish(topic, `Hello ${topic}`)
    //             console.log(`Connected to send topic: ${topic}`)
    //         }
    //     })
    // }
    // for (let i = 0; i < mqttReadTopic.length; i++) {
    //     const topic = mqttReadTopic[i]
    //     client.subscribe(topic, (err) => {
    //         if (!err) {
    //             // client.publish(topic, `Hello ${topic}`)
    //             console.log(`Connected to read topic: ${topic}`)
    //         }
    //     })
    // }
})

// handle instance where MQTT will not connect (error event)
client?.on("error", () => {
    console.log("error with mqtt")
    client = null
})

// Handle when a subscribed message comes in (message event)
client?.on("message", async (topic, message) => {
    // if (mqttReadTopic.includes(topic)) {
    let jsonData = await read()
    jsonData.push({ "id": nanoid.nanoid(10), "topic": topic, "msg": message.toString() })
    write(jsonData).then(() => console.log("success")).catch((err) => console.log(err))
    // }
})

// Route to serve the home page
app.get('/', (_, res) => {
    res.sendFile(path.resolve(__dirname, './index.html'));
});


// route to serve the JSON array from the file message.json when requested from the home page
app.get('/messages', async (req, res) => {
    const jsonData = await read()
    res.json(jsonData)
})

// Route to serve the page to add a message
app.get('/add', (req, res) => {
    res.sendFile(path.resolve(__dirname, "./message.html"))
})

//Route to show a selected message. Note, it will only show the message as text. No html needed
app.get('/:id', async (req, res) => {
    const jsonData = await read()
    const msg = jsonData.find((item) => item["id"] == req.params.id)
    res.json(msg)
})

// Route to CREATE a new message on the server and publish to mqtt broker
app.post('/', async (req, res) => {
    try {
        const { topic, msg } = req.body;
        let topics = await read("./topics.json")
        const foundTopic = topics.find((topicObj) => topicObj["name"] == topic)
        if (foundTopic && foundTopic["send"]) {
            client.publish(foundTopic["name"], msg)
            res.sendStatus(200)
        } else if (foundTopic && !foundTopic["send"]) {
            res.sendStatus(401)
        } else {
            client.subscribe(topic, (err) => {
                if (!err) {
                    topics.push({ name: topic, send: true })
                    write(topics, "./topics.json")
                    
                    client.publish(topic, msg)
                    console.log(`Connected to topic: ${topic}`)
                    res.sendStatus(200)
                } else {
                    console.log(`Error connecting to topic: ${topic}`)
                    res.sendStatus(500)
                }
            })
        }
    } catch {
        res.sendStatus(500)
    }
})

// Route to delete a message by id (Already done for you)

app.delete('/:id', async (req, res) => {
    try {
        const messages = await read();
        write(messages.filter((c) => c.id !== req.params.id));
        res.sendStatus(200);
    } catch (e) {
        res.sendStatus(200);
    }
});

// listen to the port
app.listen(PORT, () => {
    console.log(`Listening on port: ${PORT}`);
});
