# simple-config
simple custom config

## Grammar
```
Data          -> <key="" type="" value="">
DataNoKey     -> <type="" value="">
Content       -> (Data)(Content) | (Data)
ContentNoKey  -> (DataNoKey)(ContentNoKey) | (DataNoKey)
Array         -> <name>(ContentNoKey)</name>
Object        -> <name>(Content)</name>
```

## Sample Code
```cpp
akrbt::config::Value v;
v["akrbt"]["nationality"] = akrbt::config::Value::string("korea");
v["akrbt"]["gender"] = akrbt::config::Value::string("male");
v["akrbt"]["age"] = akrbt::config::Value::number(25);
v["akrbt"]["student"] = akrbt::config::Value::boolean(true);

std::vector<akrbt::config::Value> pets;
pets.push_back(akrbt::config::Value::string("choco"));
pets.push_back(akrbt::config::Value::string("navi"));
pets.push_back(akrbt::config::Value::string("bori"));

v["akrbt"]["pet"] = akrbt::config::Value::array(pets);
```

## Sample .config
```
<akrbt>
  <key="nationality" type="String" value="korea">
  <key="gender" type="String" value="male">
  <key="age" type="Number" value="25">
  <key="student" type="Boolean" value="true">
  <pet>
    <type="String" value="choco">
    <type="String" value="navi">
    <type="String" value="bori">
  </pet>
</akrbt>
```
