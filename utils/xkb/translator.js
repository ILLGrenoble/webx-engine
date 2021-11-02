import dedent from 'dedent';

export const translate = (symbolMappings, modifiers) => {

    let output = dedent`
        #ifndef WEBX_KEYBOARD_SYMBOL_MAPPING_H
        #define WEBX_KEYBOARD_SYMBOL_MAPPING_H
        
        #include "WebXKeysym.h"
    `;

    const keyMaps = [];

    output += symbolMappings.map(symbolMapping => {
        const layout = symbolMapping.layout;
        const keysymMappings = symbolMapping.symbolsToKeys;

        const webXKeysymsName = `__webx_keysyms_${layout}`;
        keyMaps.push({layout: layout, keysymsName: webXKeysymsName});

        let layoutSymbolsText = dedent`\n
            static WebXKeysym ${webXKeysymsName}[] = {\n
        `;

        layoutSymbolsText += keysymMappings.map(keysymMapping => {
            let mapping = `    { "${keysymMapping.keysymName}", 0x${keysymMapping.keysymValue.toString(16).toUpperCase()}, "${keysymMapping.scancode}", 0x${keysymMapping.keycode.toString(16).toUpperCase()}`;
            if (keysymMapping.modifiers.length > 0) {
                mapping += `, ${keysymMapping.modifiers.join(' | ')}`;
            }

            mapping += ' }';

            return mapping;
        }).join(',\n');

        layoutSymbolsText += dedent`
            \n};
        `;


        return layoutSymbolsText;
    }).join('');


    output += dedent`\n
        static const WebXKeymap WEBX_KEY_MAP[] = {\n
        `;


    output += keyMaps.map(keymap => {
        return `    { "${keymap.layout}", ${keymap.keysymsName} }`;
    }).join(',\n');

    output += dedent`
        \n};

        #endif /* WEBX_KEYBOARD_SYMBOL_MAPPING_H */
    `;

    return output;

}