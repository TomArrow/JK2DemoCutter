# Adding support for new games 
(must be q3 engine based ofc)

## Helper regex for turning itemlist into enum:

(assumes consistent formatting)

Step 1: Convert items to comma and line separated list
```
.*?{\n\t\t"([^"]+)
```
```
itemlist_$1_general,\n
```

Step 2: Convert to uppercase.

Step 3: Add ```ITEMLIST_NONE_GENERAL``` at start and ```ITEMLIST_NUM_TOTAL_GENERAL ``` at end. Done.

## Helper regex for NETF/PSF parsing

```.*?(?:NETF|PSF)\s*\(\s*([^\s)]+)\s*\),\s*([^\s}]+)```

Use $1 for fieldname and $2 for bitstuff