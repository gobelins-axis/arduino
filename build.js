// Utils
const fs = require('fs');
const yargs = require('yargs');
const md5 = require('md5');
const { exec } = require('child_process');

// Config
require('dotenv').config();

// Request options
const options = yargs
    .usage('Usage: --name <name>')
    .option('n', { alias: 'name', describe: 'Script name', type: 'string', demandOption: true })
    .option('w', { alias: 'watch', describe: 'Watch file changes', type: 'bool', demandOption: false })
    .argv;

// Setup
const fqbn = process.env.FQBN;
const name = options.name;
const path = `./scripts/${name}/${name}.ino`;

const commands = {};
commands.compile = `arduino-cli compile --fqbn ${fqbn} scripts/${name}`;

let scriptContent = md5(fs.readFileSync(path));
let allowCompile = true;

// Find arduino port
console.log('⏳ Looking for arduino board...');
exec('arduino-cli board list', (error, stdout, stderr) => {
    const arduinoPort = getArduinoPort(stdout);

    if (!arduinoPort) {
        console.error('❌ Couldnt find any arduino board connected through USB');
        return;
    }

    console.log(`✅ Board founded on port ${arduinoPort}!\n`);

    // Setup upload command
    commands.upload = `arduino-cli upload -p ${arduinoPort} --fqbn ${fqbn} scripts/${name}`;

    start();
});

function start() {
    compileAndUpload();

    if (options.watch) watch();
}

function watch() {
    console.log('Waiting for changes...');
    fs.watch(path, fileChangeHandler);
}

function fileChangeHandler(event, filename) {
    if (!allowCompile || !filename || event !== 'change') return;

    const newScriptContent = md5(fs.readFileSync(scriptPath));

    // Check if file content is newer to prevent
    // compiling on save but no changes
    if (newScriptContent !== scriptContent) {
        compileAndUpload();
    }

    scriptContent = newScriptContent;
}

function compileAndUpload() {
    if (!allowCompile) return;

    console.log('⏳ Compiling...');

    exec(commands.compile, (compileError, compileResponse) => {
        if (compileError) {
            console.error('❌ Compiling Failed');
            console.error(compileError);
            return;
        }

        console.log('✅ Compile success');
        console.log(compileResponse);

        console.log('⏳ Uploading...');

        exec(commands.upload, (uploadError, uploadResponse) => {
            if (uploadError) {
                console.error('❌ Uploading Failed');
                console.error(uploadError);
                return;
            }

            allowCompile = true;

            console.log('✅ Upload success!');
            console.log(uploadResponse);
        });
    });
}

function getArduinoPort(boardList) {
    const lines = boardList.split('\n');

    const ports = lines.filter((item) => {
        return item.includes('Serial Port');
    });
    const usbPorts = ports.filter((item) => {
        return item.includes('Serial Port (USB)');
    });

    if (usbPorts.length === 0) return;

    const port = usbPorts[0].split(' ')[0];

    return port;
}
