//marimea adaptabila a componentelor in functie de ecran
let screenWidth = window.innerWidth;
const consolaTextarea = document.querySelector('#consola');
const colect = document.querySelector('#colect');
//const copy=document.getElementById('copyToClipboard-a')


consolaTextarea.style.width = screenWidth - 90 + 'px';
colect.style.width = screenWidth - 90 + 'px';

window.addEventListener('resize', function() {
  screenWidth=window.innerWidth;
  console.log(screenWidth);
  consolaTextarea.style.width = screenWidth - 90 + 'px';
  colect.style.width = screenWidth - 90 + 'px';
});