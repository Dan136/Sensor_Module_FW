include_prefix = "									<listOptionValue builtIn=\"false\" value=\"/home/dan/Documents/Capstone/Ameba_SDK_v3.5a"
include_postfix = "\"/>"
filename = "pathlist.txt"
with open(filename) as file:
    inc_lines = file.read().splitlines()
for line in inc_lines:
    print(include_prefix+line[30:]+include_postfix)