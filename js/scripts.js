// Element Select

const pumpPowerControl =  document.querySelector("#pump-power-control");





// Functions

const initialize = () => {
    pumpPowerControl.value = 0;
}



// Events

pumpPowerControl.addEventListener("input", (e) => {
    console.log(e.target.value);
});


// Initialization

initialize();