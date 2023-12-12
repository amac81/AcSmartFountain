// Element Select
const color = document.querySelector("#color");
const ledStripHEXColor = document.querySelector("#ledStripHEXColor");
const ledStripRGBColor = document.querySelector("#ledStripRGBColor");

// global variables


// Functions

function updateSliderPWM(element) {
  let motorSliderValue = document.getElementById("pwmMotorSlider").value;
  document.getElementById("textMotorSliderValue").innerHTML = motorSliderValue;
  let xhr = new XMLHttpRequest();
  xhr.open("GET", "/motorslider?value="+motorSliderValue, true);
  xhr.send();
}

function updateRgbColor(value) {
  let xhr = new XMLHttpRequest();
  xhr.open("GET", "/ledstripcolor?value="+value, true);
  xhr.send();
}

function updateBrigthness(element) {
  let brightnessValue = document.getElementById("brightnessSlider").value;
  document.getElementById("textBrightnessSliderValue").innerHTML = brightnessValue;
  let xhr = new XMLHttpRequest();

  xhr.open("GET", "/brigthness?value="+brightnessValue, true);
  xhr.send();
}

function hexToRgb (hex) {
  return `${("0x"+ hex[1] + hex[2]) | 0}_${("0x" + hex[3] + hex[4]) | 0}_${("0x" + hex[5] + hex[6]) | 0}`;
}

// Events

color.addEventListener("input", (e) => {
  const value = e.target.value;
  const rgbColor = hexToRgb(value);

  ledStripHEXColor.innerHTML = value;
  ledStripRGBColor.innerHTML = rgbColor.replaceAll("_", ", ");
  updateRgbColor(rgbColor);
});
