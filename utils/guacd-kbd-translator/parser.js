import * as fs from 'fs';

export const modifiers = {
    SHIFT_MODIFIER_KEY: 'WEBX_SHIFT_MODIFIER_KEY',
    ALTGR_MODIFIER_KEY: 'WEBX_ALTGR_MODIFIER_KEY',
    CAPS_LOCK_KEY: 'WEBX_CAPS_LOCK_KEY',
    NUM_LOCK_KEY: 'WEBX_NUM_LOCK_KEY'
} 


const convertToModifier = (text) => {
    if (text === 'caps') {
        return {isLock: true, value: modifiers.CAPS_LOCK_KEY};
    } else if (text === 'num') {
        return {isLock: true, value: modifiers.NUM_LOCK_KEY};
    } else if (text === 'shift') {
        return {isLock: false, value: modifiers.SHIFT_MODIFIER_KEY};
    } else if (text === 'altgr') {
        return {isLock: false, value: modifiers.ALTGR_MODIFIER_KEY};
    }
}

const guacdSymbols = [{
    layout: 'gb',
    name: 'en-gb-qwerty',
    file: 'en_gb_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'hu',
    name: 'hu-hu-qwertz',
    file: 'hu_hu_qwertz.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'br',
    name: 'pt-br-qwerty',
    file: 'pt_br_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'dk',
    name: 'da-dk-qwerty',
    file: 'da_dk_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'us',
    name: 'en-us-qwerty',
    file: 'en_us_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'be',
    name: 'fr-be-azerty',
    file: 'fr_be_azerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'it',
    name: 'it-it-qwerty',
    file: 'it_it_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'se',
    name: 'sv-se-qwerty',
    file: 'sv_se_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'ch(de)',
    name: 'de-ch-qwertz',
    file: 'de_ch_qwertz.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'es',
    name: 'es-es-qwerty',
    file: 'es_es_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'ch(fr)',
    name: 'fr-ch-qwertz',
    file: 'fr_ch_qwertz.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'jp',
    name: 'ja-jp-qwerty',
    file: 'ja_jp_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'tr',
    name: 'tr-tr-qwerty',
    file: 'tr_tr_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'de',
    name: 'de-de-qwertz',
    file: 'de_de_qwertz.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'latam',
    name: 'es-latam-qwerty',
    file: 'es_latam_qwerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'fr',
    name: 'fr-fr-azerty',
    file: 'fr_fr_azerty.keymap',
    inherits: ['base.keymap']
}, {
    layout: 'no',
    name: 'no-no-qwerty',
    file: 'no_no_qwerty.keymap',
    inherits: ['base.keymap']
}];

const parseSymbolFile = (file) => {
    console.log(`Parsing ${file}`);
    const data = fs.readFileSync(`mappings/${file}`, 'utf8');

    return data.split('\n')
        .filter(s => s.length > 0)
        .map(line => {
            const pattern = /^\s*map\s+([^~]*)\s+~\s+(".*"|0x[0-9A-Fa-f]+)\s*(?:#.*)?$/;
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        })
        .filter(matches => matches != null)
        .flatMap(matches => {
            const options = matches[1].split(/\s+/);
            const onto = matches[2];
            const modifiersSet = [];
            const locksSet = [];
            const modifiersClear = [];
            const locksClear = [];
            const rdpScancodes = [];
            const charVals = [];

            // Handle options
            for (const option of options) {
                let valueExpr;
                if (valueExpr = new RegExp(/^(-|\+)([a-z]+)/, 'g').exec(option)) {
                    const plusOrMinus = valueExpr[1];
                    const modifierType = valueExpr[2];
                    const modifier = convertToModifier(modifierType);
                    if (modifier) {
                        if (plusOrMinus === '+' && modifier.isLock) {
                            locksSet.push(modifier.value);
                        
                        } else if (plusOrMinus === '-' && modifier.isLock) {
                            locksClear.push(modifier.value);
                        
                        } else if (plusOrMinus === '+' && !modifier.isLock) {
                            modifiersSet.push(modifier.value);
                        
                        } else if (plusOrMinus === '-' && !modifier.isLock) {
                            modifiersClear.push(modifier.value);
                        }
                    }

                } else if (valueExpr = new RegExp(/^(0x[0-9A-Fa-f]+)$/, 'g').exec(option)) {
                    const rdpScancode = parseInt(option);
                    rdpScancodes.push(rdpScancode);

                } else if (valueExpr = new RegExp(/^(0x[0-9A-Fa-f]+)\.\.(0x[0-9A-Fa-f]+)$/, 'g').exec(option)) {
                    const rdpScancodeStart = parseInt(valueExpr[1]);
                    const rdpScancodeEnd = parseInt(valueExpr[2]);
                    for (let i = rdpScancodeStart; i <= rdpScancodeEnd; i++) {
                        rdpScancodes.push(i);
                    }
                }
            }

            // Handle onto
            if (new RegExp(/^0x/, 'g').exec(onto)) {
                charVals.push(parseInt(onto));

            } else {
                for (let i = 1; i < onto.length - 1; i++) {
                    let ascii = onto.charCodeAt(i);
                    // if (ascii >= 0x0100) {
                        // ascii |= 0x01000000;
                    // }

                    charVals.push(ascii);
                }
            }
            
            // Check number of keysyms and rdpScancodes
            if (charVals.length !== rdpScancodes.length) {
                console.error(`Keysym and RDP scan code lengths differ for line '${matches[0]}' in file '${file}'`);
                return null
            }

            return charVals.map((charVal, index) => {
                return {charVal, rdpScancode: rdpScancodes[index], modifiersSet, locksSet, modifiersClear, locksClear};
            });
        })
        .filter(parse => parse != null);
}


export const parseSymbols = () => {
    const parsed = guacdSymbols.map(guacdSymbol => {
        const { layout, name, file, inherits } = guacdSymbol;
        const symbols = parseSymbolFile(file);
    
        if (inherits && inherits.length > 0) {
            inherits.forEach(inherited => {

                parseSymbolFile(inherited).forEach(key => {
                    symbols.push(key);      
                });
            });
        }
    
        return { layout, name, symbols }
    });

    return parsed;
}

export const parseKeySymDefFile = () => {
    const data = fs.readFileSync(`mappings/keysymdef.h`, 'utf8');
    return data.split('\n')
        .filter(s => s.length > 0)
        .map(line => {
            const pattern = /#define XK_(\w+)\s+(0x\w+)(?:.*U\+(\w+))?/;
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        })
        .filter(matches => matches != null)
        .map(matches => {
            const name = matches[1];
            const keysymHex = matches[2];
            const unicode = matches[3];
            const value = parseInt(keysymHex);
            return { name, value, unicode: unicode ? parseInt(`0x${unicode}`) : null };
        });
}