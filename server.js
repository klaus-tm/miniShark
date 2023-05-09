const http = require('http');
const path = require('path');
const fs = require('fs');
const formidable = require('formidable');
const { spawn } = require('child_process');

const server = http.createServer((req, res) => {
   if (req.url === '/upload' && req.method.toLowerCase() === 'post') { //3
      const form = new formidable.IncomingForm();
      form.parse(req, (err, fields, files) => { //2
         if (err) {
            console.error(err);
            res.statusCode = 500;
            res.end('Error uploading file');
            return;}
         
         const file = files.file;
         if (!file || file.type !== 'application/vnd.tcpdump.pcap') {
            res.statusCode = 400;
            res.end('Invalid file type');
            return;}

         const oldPath = file.path;
         const newPath = './uploads/' + file.name;
         const conexiune = fields.conexiune; 
         const so = fields.so; 

         fs.rename(oldPath, newPath, (err) => { //1
            if (err) {
            console.error(err);
            res.statusCode = 500;
            res.end('Error saving file');
            return;}

            const human_readable=document.getElementById('#human_readable');
            human_readable.addEventListener('click', function(){
               //generează procesul copil și trece calea fișierului și alte argumente ca argumente
               const childProcess = spawn('./human_readable', [newPath, conexiune, so]);

               //asteapta datele din proces
               childProcess.stdout.on('data', (data) => {
                 console.log(`Received data from child process: ${data}`);
                 res.write(data); // write the data from the child process to the response
               });

               //asteapta dupa erori de la procesul copil
               childProcess.stderr.on('data', (data) => {
                 console.error(`Error from child process: ${data}`);
               });

               //trimite la procesul copil
               //childProcess.stdin.write('Hello from server.js!\n');

               res.statusCode = 200;
               res.setHeader('Content-Type', 'text/plain');
               res.end('File uploaded successfully');
            });

            const hex=document.getElementById('#hex');
            hex.addEventListener('click', function(){
               //generează procesul copil și trece calea fișierului și alte argumente ca argumente
               const childProcess = spawn('./hex', [newPath, conexiune, so]);

               //asteapta datele din proces
               childProcess.stdout.on('data', (data) => {
                 console.log(`Received data from child process: ${data}`);
                 res.write(data); // write the data from the child process to the response
               });

               //asteapta dupa erori de la procesul copil
               childProcess.stderr.on('data', (data) => {
                 console.error(`Error from child process: ${data}`);
               });

               //trimite la procesul copil
               //childProcess.stdin.write('Hello from server.js!\n');

               res.statusCode = 200;
               res.setHeader('Content-Type', 'text/plain');
               res.end('File uploaded successfully');
            });

            const capture=document.getElementById('#capture');
            capture.addEventListener('click', function(){
            //generează procesul copil și trece calea fișierului și alte argumente ca argumente
            const childProcess = spawn('./capture', [newPath, conexiune, so]);

            //asteapta datele din proces
            childProcess.stdout.on('data', (data) => {
              console.log(`Received data from child process: ${data}`);
              res.write(data); // write the data from the child process to the response
            });

            //asteapta dupa erori de la procesul copil
            childProcess.stderr.on('data', (data) => {
              console.error(`Error from child process: ${data}`);
            });

            res.statusCode = 200;
            res.setHeader('Content-Type', 'text/plain');
            res.end('File uploaded successfully');});
         }); //1
      }); //2
   }//3

   else { //5
      let filePath = '.' + req.url;

      if (filePath === './')
         filePath = './home.html';
    

      const extname = String(path.extname(filePath)).toLowerCase();
      const mimeTypes = {
         '.html': 'text/html',
         '.js': 'text/javascript',
         '.css': 'text/css',
         '.json': 'application/json',
         '.png': 'image/png',
         '.jpg': 'image/jpg',
         '.gif': 'image/gif',
         '.svg': 'image/svg+xml',
         '.wav': 'audio/wav',
         '.mp4': 'video/mp4',
         '.woff': 'application/font-woff',
         '.ttf': 'application/font-ttf',
         '.eot': 'application/vnd.ms-fontobject',
         '.otf': 'application/font-otf',
         '.wasm': 'application/wasm'
       };
      const contentType = mimeTypes[extname] || 'application/octet-stream';

      fs.readFile(filePath, function(error, content) { //4
         if (error) {
            if (error.code == 'ENOENT') {
               fs.readFile('./404.html', function(error, content) {
                  res.writeHead(404, { 'Content-Type': contentType });
                  res.end(content, 'utf-8');
               });
            }
            else {
               res.writeHead(500);
               res.end('Sorry, check with the site admin for error: '+error.code+' ..\n');
               res.end();}
         }
         else {
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(content, 'utf-8');}
      }); //4
   } //5
});

server.listen(3000, () => {
  console.log('Server running at http://localhost:3000/');
});
