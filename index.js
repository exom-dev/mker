var binding = require('./build-load')(__dirname);

module.exports = (source) => {
    if(!(source && (typeof source === 'string' || (source instanceof String))))
        throw `Invalid argument 'source' (expected: string | found: ${typeof(source)})`;

    return binding.compile(source);
}