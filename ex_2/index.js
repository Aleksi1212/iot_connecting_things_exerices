const express = require('express');
const mqtt = require('mqtt');
const path = require('path');
const util = require('util');
const fs = require('fs');
const bodyParser = require('body-parser');
const app = express();

const PORT = 3000;

const readFile = util.promisify(fs.readFile);
const writeFile = util.promisify(fs.writeFile);
// Create variables for MQTT use here

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


// Check that you are connected to MQTT and subscribe to a topic (connect event)

// handle instance where MQTT will not connect (error event)

// Handle when a subscribed message comes in (message event)

// Route to serve the home page
app.get('/', (_, res) => {
    res.sendFile(path.resolve(__dirname, './index.html'));
});

// route to serve the JSON array from the file message.json when requested from the home page

// Route to serve the page to add a message

//Route to show a selected message. Note, it will only show the message as text. No html needed

// Route to CREATE a new message on the server and publish to mqtt broker

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
