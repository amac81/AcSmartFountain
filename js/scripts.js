// Element Select

const pumpPowerControl =  document.querySelector("#pump-power-control");

const fogImg =  document.querySelector("#fog-img");
const waterStreamImg1 =  document.querySelector("#water-stream-img1");
const waterStreamImg2 =  document.querySelector("#water-stream-img2");
const waterStreamImg3 =  document.querySelector("#water-stream-img3");
const waterStreamImg4 =  document.querySelector("#water-stream-img4");

// global variables

let myTimer1;

// Functions




const initialize = () => {
    //pumpPowerControl.value = 10;
//    controlWaterStreamsFlow();

}

const animeWaterStream = (waterStreamElem, time) => {
    
    let currentOpacity = getComputedStyle(waterStreamElem).getPropertyValue("opacity");
    const changeFactor = 0.05;

    console.log(time)

    myTimer1 = setInterval(() => {

        if(currentOpacity < 0){
            currentOpacity = 0.98;
        }
        else {
            currentOpacity -= changeFactor;        
            waterStreamElem.setAttribute("style",`opacity:${currentOpacity}`) ;           
        }
        
    }, time);

}

const controlWaterStreamsFlow = (flowFactor) => {
   
    const time1 = 45 + (+flowFactor);
    /*const time2 = 255 - (+flowFactor);
    const time3 = 265 - (+flowFactor);
    const time4 = 275 - (+flowFactor);*/
    

    /*clearInterval(myTimer2);
    clearInterval(myTimer3);
    clearInterval(myTimer4);*/
    
    animeWaterStream(waterStreamImg1, time1);
    /*animeWaterStream(waterStreamImg2, myTimer2, time2);
    animeWaterStream(waterStreamImg3, myTimer3, time3);
    animeWaterStream(waterStreamImg4, myTimer4, time4);*/    
};


// Events

pumpPowerControl.addEventListener("input", (e) => {
    clearInterval(myTimer1);
    
    const value = e.target.value;

    //console.log(`Control Value = ${e.target.value}`);
  //  console.log(`Value to send = ${value}`);

    controlWaterStreamsFlow(value)

});


// Initialization

initialize();


