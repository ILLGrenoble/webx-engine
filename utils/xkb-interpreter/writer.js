import * as fs from 'fs';

export const write = (filename, content) => {

    try {
        fs.writeFileSync(filename, content);

        console.log(`C++ keyboard symbol mapping written to ${filename}`);
    } catch (err) {
        console.log(`Could not write keyboard symbol mapping to ${filename}: ${err.message}`);
    }
}