const humanReadable = document.querySelector('#human_readable');
const consolaTextarea = document.querySelector('#consola');
//const fileObj=document.querySelector('#file');
const save=document.querySelector('#save');
const consola=document.querySelector('#consola');

let filePath;

function verificaCaleFisier(cale) {
  return fetch(cale, { method: 'HEAD' })
    .then(response => {
      if (response.ok) {
        return true; // Fișierul există
      } else {
        return false; // Fișierul nu există
      }
    })
    .catch(error => {
      console.error('Network error:', error);
      throw error;
    });
}


function loadFile(e){
  filePath = document.querySelector('#cale').value;
  filePath=filePath.replace(/\\/g, '\\\\');

  if (filePath.endsWith('.pcap')){
    if(verificaCaleFisier(filePath)){
      const eroareCell = document.getElementById('eroare');
      eroareCell.textContent = '';
      //salveaza fisierul local si ii ia calea
      humanReadable.disabled = false;
    }
    else{
      const eroareCell = document.getElementById('eroare');
      eroareCell.textContent = 'Your file is not in PCAP format. Load a PCAP file.';
      humanReadable.disabled = true;
    }
    
  }
  else{
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = 'That is not a correct path.';
    humanReadable.disabled = true;
  }
}

function humanFct(e) {
  if (filePath) {
    window.electronAPI.sendFileToMainProcess('HR',filePath);
  
    // Ascultarea răspunsului de la procesul principal
    window.electronAPI.receiveProcessedFilePath((filePath) => {
      console.log('Calea absolută a fișierului procesat:', filePath);
    });
  }
}

//fileObj.addEventListener('change',loadFile);
save.addEventListener('click',loadFile);

humanReadable.addEventListener('click', humanFct);