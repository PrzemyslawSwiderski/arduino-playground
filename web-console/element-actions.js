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

document.getElementById('reset-cam-button')
    .addEventListener('mousedown', (e) => {
        sendCmd('reset-cam');
    });



// JPEG QUALITY

const qualitySlider = document.getElementById('quality-slider');
const qualityValue = document.getElementById('quality-value');

function setQuality(quality) {
    qualityValue.textContent = quality;
    qualitySlider.value = quality;
    sendCmd(`quality:${quality}`);
}

qualitySlider.addEventListener('input', (e) => {
    const newQuality = parseInt(e.target.value, 10);
    setQuality(newQuality);
});

const defaultQuality = 30;
setQuality(defaultQuality);

// BRIGHTNESS QUALITY

const brightnessSlider = document.getElementById('brightness-slider');
const brightnessValue = document.getElementById('brightness-value');

function setBrightness(brightness) {
    brightnessValue.textContent = brightness;
    brightnessSlider.value = brightness;
    sendCmd(`brightness:${brightness}`);
}

brightnessSlider.addEventListener('input', (e) => {
    const newBrightness = parseInt(e.target.value, 10);
    setBrightness(newBrightness);
});

const defaultBrightness = -1;
setBrightness(defaultBrightness);


// Frame Size

const sizeSlider = document.getElementById('size-slider');
const sizeValue = document.getElementById('size-value');

function setSize(size) {
    sizeValue.textContent = size;
    sizeSlider.value = size;
    sendCmd(`size:${size}`);
}

sizeSlider.addEventListener('input', (e) => {
    const newSize = parseInt(e.target.value, 10);
    setSize(newSize);
});

const defaultSize = 4;
setSize(defaultSize);


// Contrast

const contrastSlider = document.getElementById('contrast-slider');
const contrastValue = document.getElementById('contrast-value');

function setContrast(contrast) {
    contrastValue.textContent = contrast;
    contrastSlider.value = contrast;
    sendCmd(`contrast:${contrast}`);
}

contrastSlider.addEventListener('input', (e) => {
    const newContrast = parseInt(e.target.value, 10);
    setContrast(newContrast);
});

const defaultContrast = 0;
setContrast(defaultContrast);
