import { sendCmd } from "./websockets.js";


document.getElementById('light-on')
    .addEventListener('mousedown', (e) => {
        sendCmd('ledon');
    });

document.getElementById('light-off')
    .addEventListener('mousedown', (e) => {
        sendCmd('ledoff');
    });


document.getElementById('get-info-button')
    .addEventListener('mousedown', (e) => {
        sendCmd('info');
    });


const consoleBox = document.getElementById('console-box');

document.getElementById('clear-console-button')
    .addEventListener('mousedown', (e) => {
        consoleBox.innerHTML = '';
    });

document.getElementById('reset-button')
    .addEventListener('mousedown', (e) => {
        sendCmd('reset');
    });

document.getElementById('wifi-switch-button')
    .addEventListener('mousedown', (e) => {
        sendCmd('wifi-change');
    });
