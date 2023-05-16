//marimea adaptabila a componentelor in functie de ecran
let screenWidth = window.innerWidth;
const consolaTextarea = document.querySelector('#consola');
const colect = document.querySelector('#colect');
const filtreID=document.querySelector('#filtreID');

consolaTextarea.style.width = screenWidth - 90 + 'px';
colect.style.width = screenWidth - 90 + 'px';
filtreID.style.width = screenWidth - 90 + 'px';

window.addEventListener('resize', function() {
  screenWidth=window.innerWidth;
  consolaTextarea.style.width = screenWidth - 90 + 'px';
  colect.style.width = screenWidth - 90 + 'px';
  filtreID.style.width = screenWidth - 90 + 'px';});

const humanReadable = document.querySelector('#human_readable');
const fileObj=document.querySelector('#file');
const consola=document.querySelector('#consola');

function loadFile(e){
  file=e.target.files[0];

  if (file.name.endsWith('.pcap')){
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = '';
    humanReadable.disabled = false;
  }
  else{
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = 'Your file is not in PCAP format. Load a PCAP file.';
    humanReadable.disabled = true;
  }
}

function formatData(data) {
  let result = '';

  for (const key in data) {
    if (data.hasOwnProperty(key)) {
      result += `${key} - ${data[key]}\n`;
    }
  }

  return result;
}

let s='';

function humanFct(e) {
  const currentURL = window.location.pathname;
  let filePath = currentURL.substring(0, currentURL.lastIndexOf('/'));
  filePath = filePath.substring(filePath.indexOf('/') + 1) + '/json/packet';
  let promises = [];

  for (let i = 0; i < 8; i++) {
    let currentFilePath = filePath + (i + 1) + '.json';

    // Fetch JSON data for the current file
    let promise = fetch(currentFilePath)
      .then(response => response.json())
      .then(data => {
        s += formatData(data);
        s += "\n\n\n";
      })
      .catch(error => {
        console.error('An error occurred:', error);
      });

    promises.push(promise);
  }

  Promise.all(promises)
    .then(() => {
      consolaTextarea.value = s;
    });
}

fileObj.addEventListener('change',loadFile);

humanReadable.addEventListener('click', humanFct);