// utils/valueConversion.js

function convertValueToString(value) {
  const convertMap = {
    string: (v) => `${v.string}`,
    bool: (v) => `${v.bool}`,
    int32: (v) => `${v.int32}`,
    int64: (v) => `${v.int64}`,
    uint8: (v) => `${v.uint8}`,
    uint16: (v) => `${v.uint16}`,
    uint32: (v) => `${v.uint32}`,
    uint64: (v) => `${v.uint64}`,
    float: (v) => `${v.float}`,
    double: (v) => `${v.double}`,
    string_array: (v) => `${v.string_array.join(",")}`,
    bool_array: (v) => `${v.bool_array.join(",")}`,
    int32_array: (v) => `${v.int32_array.join(",")}`,
    int64_array: (v) => `${v.int64_array.join(",")}`,
    uint32_array: (v) => `${v.uint32_array.join(",")}`,
    uint64_array: (v) => `${v.uint64_array.join(",")}`,
    float_array: (v) => `${v.float_array.join(",")}`,
    double_array: (v) => `${v.double_array.join(",")}`,
    value_object: (v) => `${v.value_object.join(",")}`,
  };

  const key = Object.keys(value).find((k) => convertMap[k]);
  if (key) return convertMap[key](value);
  return "Unknown value";
}

module.exports = {
  convertValueToString,
};
