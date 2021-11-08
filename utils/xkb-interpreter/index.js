import yargs from 'yargs'
import { hideBin } from 'yargs/helpers'
import { parseKeycodeFile, parseKeySymDefFile, parseSymbols } from './parser.js'
import { interpret } from './interpreter.js'
import { translate } from './translator.js'
import { write } from './writer.js'

const argv = yargs(hideBin(process.argv))
    .option('outfile', {
        alias: 'o',
        type: 'string',
        description: 'Path to the output file'
    })
    .demandOption(['outfile'])
    .parse();


const outfile = argv.outfile;

const symbols = parseSymbols();
const keycodes = parseKeycodeFile();
const keysymdefs = parseKeySymDefFile();

const symbolMappings = interpret(symbols, keycodes, keysymdefs);

const translation = translate(symbolMappings);

write(outfile, translation);

// console.log(translation);