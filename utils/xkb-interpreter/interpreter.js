export const modifiers = {
  MODIFIER_SHIFT_KEY: 'WEBX_MODIFIER_SHIFT_KEY',
  MODIFIER_ALTGR_KEY: 'WEBX_MODIFIER_ALTGR_KEY'
} 

const MODIFIERS = [
  [],
  [modifiers.MODIFIER_SHIFT_KEY],
  [modifiers.MODIFIER_ALTGR_KEY],
  [modifiers.MODIFIER_SHIFT_KEY, modifiers.MODIFIER_ALTGR_KEY],
]

export const interpret = (symbols, keycodeMappings, keysymDefs) => {
  // Map the original symbol definitions for the keyboard layouts
  const transformedSymbols = symbols.map(({layout, symbols}) => {

    // For each keysym, generate a keycode and modifier mapping 
    const symbolsToKeys = symbols.flatMap(({scancode, keysyms}) => {

      // For the keysym, convert the keysym name to a keysym value, convert the scancode to a keycode, and get the modifier keys
      const keysymMappings = keysyms.map((keysymName, index) => {
        // Modifier keys are dependent on the index in array
        const modifiers = MODIFIERS[index]

        // Get keysym value for the keysym name
        const keysym = keysymDefs.find(({name}) => name === keysymName);
        if (keysym == null) {
          if (keysymName !== 'NoSymbol') {
            console.error(`Could not find keysym value for keysym name '${keysymName}'`);
          }
          return null;
        }
        const keysymValue = keysym.value;

        // Convert the scancode to a keycode
        const keycodeMapping = keycodeMappings.find((keycodeMapping => keycodeMapping.scancode === scancode));
        if (keycodeMapping == null) {
          console.error(`Could not find keycode for scancode '${scancode}'`);
          return null;
        }
        const keycode = keycodeMapping.keycode;

        // Return keysym mapping
        return {keysymName, keysymValue, scancode, keycode, modifiers};

      }).filter(keysymMapping => keysymMapping != null);

      return keysymMappings;
    });

    return {layout, symbolsToKeys};
  });

  return transformedSymbols;
}