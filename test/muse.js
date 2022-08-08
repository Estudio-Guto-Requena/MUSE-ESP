require('./text-polyfill');
const { MUSE_SERVICE, MuseClient, zipSamples, channelNames } = require('muse-js');
const noble = require('noble');
const bleat = require('bleat').webbluetooth;
const lsl = require('node-lsl');
const ansi = require('ansi')
, cursor = ansi(process.stdout)

function print_on_screen(eeg) {
  cursor.horizontalAbsolute(0).eraseLine().write(JSON.stringify(eeg, null, 2))
}

async function connect() {
    let device = await bleat.requestDevice({
        filters: [{ services: [MUSE_SERVICE] }]
    });
    const gatt = await device.gatt.connect();
    console.log('Device name:', gatt.device.name);

    const client = new MuseClient();
    await client.connect(gatt);
    client.controlResponses.subscribe(control => console.log('Response:', control));
    //client.telemetryData.subscribe(telemetry => console.log('Telemetry:',telemetry));
    //client.accelerometerData.subscribe(acceleration => console.log('Acceleration:',acceleration));
    client.eegReadings.subscribe(eeg => print_on_screen(eeg));
    await client.start();
    console.log('Connected!');
    return client;
}

noble.on('stateChange', (state) => {
  console.log("Ready");
    if (state === 'poweredOn') {
        connect();
    }
});
