// Element Select
const colorPicker = document.querySelector("#colorPicker");
const ledStripHEXColor = document.querySelector("#ledStripHEXColor");
const ledStripRGBColor = document.querySelector("#ledStripRGBColor");
const ledRgbValue = document.querySelector("#ledRgbValue");
const pumpPowerSlider = document.querySelector("#pumpPowerSlider");
const pumpPowerSliderValue = document.querySelector("#pumpPowerSliderValue");
const brightnessSlider =  document.querySelector("#brightnessSlider");
const brightnessSliderValue = document.querySelector("#brightnessSliderValue");
const mistModeSlider =  document.querySelector("#mistModeSlider");
const mistModeSliderValue = document.querySelector("#mistModeSliderValue");

// Functions

function XMLHttpRequestSend(path, value){
  let xhr = new XMLHttpRequest();
  xhr.open("GET", path + value, true);
  xhr.send();
}

function updatePumpPower(value) {
  pumpPowerSliderValue.innerHTML = value;
  XMLHttpRequestSend("/pumppower?value=", value);
}

//https://stackoverflow.com/questions/5623838/rgb-to-hex-and-hex-to-rgb
const rgbToHex = (r, g, b) => '#' + [parseInt(r), parseInt(g), parseInt(b)].map(x => x.toString(16).padStart(2, '0')).join('')

function updateColorInfo(value){
  const rgbColor = value.replaceAll("_", ", ");
  
  ledStripRGBColor.innerHTML = rgbColor;

  let rgb = []; 
  rgb = value.split("_"); 
  
  const hexColor = rgbToHex(rgb[0], rgb[1], rgb[2]);

  ledStripHEXColor.innerHTML = hexColor;
  colorPicker.value = hexColor;
}

function updateRgbColor(value) {
  const rgbColor = hexToRgb(value);
  ledStripHEXColor.innerHTML = value;
  ledStripRGBColor.innerHTML = rgbColor.replaceAll("_", ", ");
  XMLHttpRequestSend("/ledstripcolor?value=", rgbColor);
}

function updateBrigthness(value) {
  brightnessSliderValue.innerHTML = value;
  XMLHttpRequestSend("/brigthness?value=", value);
}

function updateMistMode(value) {
  mistModeSliderValue.innerHTML = value;
  XMLHttpRequestSend("/mistmode?value=", value);
}

function hexToRgb (hex) {
  return `${("0x"+ hex[1] + hex[2]) | 0}_${("0x" + hex[3] + hex[4]) | 0}_${("0x" + hex[5] + hex[6]) | 0}`;
}

function init (){
  updateColorInfo(ledRgbValue.innerHTML);
}

// Events

colorPicker.addEventListener("input", (e) => {
  const value = e.target.value;
  updateRgbColor(value);
});

brightnessSlider.addEventListener("input", (e) => {
  updateBrigthness(e.target.value);
});

pumpPowerSlider.addEventListener("input", (e) => {
  updatePumpPower(e.target.value);
});

mistModeSlider.addEventListener("input", (e) => {
  updateMistMode(e.target.value);
});

init();