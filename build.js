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

const port = process.env.PORT;
const fqbn = process.env.FQBN;
const name = options.name;
const path = `./scripts/${name}/${name}.ino`;

const commandCompile = `arduino-cli compile --fqbn ${fqbn} scripts/${name}`;
const commandUpload = `arduino-cli upload -p ${port} --fqbn ${fqbn} scripts/${name}`;

let scriptContent = md5(fs.readFileSync(path));
let allowCompile = true;

function setup() {
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

    exec(commandCompile, (compileError, compileResponse) => {
        if (compileError) {
            console.error('❌ Compiling Failed');
            console.error(compileError);
            return;
        }

        console.log('✅ Compile success');
        console.log(compileResponse);

        console.log('⏳ Uploading...');

        exec(commandUpload, (uploadError, uploadResponse) => {
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

setup();
