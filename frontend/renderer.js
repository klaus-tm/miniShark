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


function transform_hr(json,show) {
  //pentru fiecare json
  let ethernet='Ethernet:\n';
  let ip='IP:\n';
  let packet='Packet:\n';
  let tcp='TCP:\n';
  let udp='UDP:\n';
  let icmp='ICMP:\n';
  let arp='ARP:\n';

  for (const [key, value] of Object.entries(json)) {
    if (key.includes("Ethernet")){
      const modifiedKey = key.replace("Ethernet ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!ethernet.includes(line))
        ethernet += line;
    }
    if (key.includes("IP")){
      const modifiedKey = key.replace("IP ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!ip.includes(line))
        ip += line;
    }
    if (key.includes("Packet")){
      const modifiedKey = key.replace("Packet ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!packet.includes(line))
        packet += line;
    }
    if (key.includes("TCP")){
      const modifiedKey = key.replace("TCP ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!tcp.includes(line))
        tcp += line;
    }
    if (key.includes("UDP")){
      const modifiedKey = key.replace("UDP ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!udp.includes(line))
        udp += line;
    }
    if (key.includes("ICMP")){
      const modifiedKey = key.replace("ICMP ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!icmp.includes(line))
        icmp += line;
    }
    if (key.includes("ARP")){
      const modifiedKey = key.replace("ARP ", "");
      const line = '\t' + modifiedKey + ' - '+ value + '\n';
      if(!arp.includes(line))
        arp += line;
    }
  }

  let content='';

  if (ethernet!='Ethernet:\n' && (show=='All' || show=='Ethernet'))
    content += ethernet;
  if (ip!='IP:\n' && (show=='All' || show=='IP'))
    content += ip;
  if (packet!='Packet:\n' && (show=='All' || show=='Packet'))
    content += packet;
  if (tcp!='TCP:\n' && (show=='All' || show=='TCP'))
    content += tcp;
  if(udp!='UDP:\n' && (show=='All' || show=='UDP'))
    content +=udp;
  if(icmp!='ICMP:\n' && (show=='All' || show=='ICMP'))
    content +=icmp;
  if(arp!='ARP:\n' && (show=='All' || show=='ARP'))
    content +=arp;

  return content;
}


function readJson(n) {
  const show =document.getElementById('special_filter').value; 
  let content='';
  const jsonPath = "../backend/output/packet";

  for (let i = 1; i <= n; ++i) {
    const jsonPathPack = jsonPath + i + ".json";
    fetch(jsonPathPack)
      .then(response => response.json())
      .then(data => {;

        content += transform_hr(data,show);
        if(content) content +='\n';

      })
      .catch(error => {
        console.error(error);
      });
  }

  setTimeout(() => {
    //console.log(content);
    if(!content){
      if(show =='All') content='Sorry. We have a problem. Try later.';
      else content='We have nothing to show about '+ show +'.';
    }
    consola.value=content;
  }, 1000); // Se așteaptă 1 secundă pentru a permite finalizarea cererilor asincrone
}


function humanFct(e) {
  if (filePath) {
    window.electronAPI.sendFileToMainProcess('HR',filePath);
  
    // Ascultarea răspunsului de la procesul principal
    window.electronAPI.receiveProcessedFilePath((n) => {
      console.log('Renderer - Nr de pachete json: ', n);

      readJson(n);
    });
  }
}





save.addEventListener('click',loadFile);

humanReadable.addEventListener('click', humanFct);