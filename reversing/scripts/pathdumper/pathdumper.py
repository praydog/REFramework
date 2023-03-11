from __future__ import print_function
import frida
import sys
import regex
import os

if len(sys.argv) < 2:
    # request input from user
    print("Enter the process name, e.g. re2.exe:")

    for line in sys.stdin:
        line = line.strip()
        if line:
            sys.argv.append(line)
            break
    

print("Waiting for {}".format(sys.argv[1]))

while True:
    try:
        session = frida.attach("{}".format(sys.argv[1]))
        print("Attached to {}".format(sys.argv[1]))
        break
    except frida.ProcessNotFoundError:
        continue

script = session.create_script("""
console.log("Searching for pattern...");

const m = Process.enumerateModules()[0];
const patterns = ['40 55 41 54 41 55 41 57 48 8D AC 24 ?? ?? ?? ?? B8', // RE2/RE3/DMC5/GnG no RT
                  '40 55 53 41 55 48 8d AC 24 ?? ?? ?? ?? B8 50 18 00 00', // RE7 no RT
                  '40 55 53 41 56 48 8d ac 24 ?? ?? ?? ?? b8 40 10 00 00', // RE7/RE2/RE3 RT
                  '40 55 53 41 56 48 8d ac 24 ?? ?? ?? ?? b8 50 10 00 00', // MHRise Gamepass
];

for (var i = 0; i < patterns.length; i++) {
    var pattern = patterns[i];

    console.log("Searching for pattern: " + pattern);

    const results = Memory.scanSync(m.base, m.size, pattern);

    if (results.length == 0) {
        continue;
    }

    console.log("Memory.scanSync() result:" + JSON.stringify(results));

    if (results.length > 0) {
        Interceptor.attach(results[0].address, {
            onEnter: function(args) {
                // Some games pass the arg in RDX instead of RCX
                // so we will just pass both args
                // since we use regex, it won't screw anything up.
                send(Memory.readUtf16String(args[0]));
                send(Memory.readUtf16String(args[1]));
            }
        })

        console.log("Hooked!");
    } else {
        console.log('No matches found. Cannot hook.');
    }
}
""")

r = regex.compile(r'(.*?\/(natives.*?)(\..*))')
seen_files = {}

# Load existing files.list
if os.path.isfile("{}_files.list".format(sys.argv[1])):
    with open("{}_files.list".format(sys.argv[1]), "r") as f:
        for line in f:
            seen_files[line.strip()] = True

def on_message(message, data):
    if not "payload" in message:
        return

    payload = message["payload"]
    if payload:
        match = r.match(payload)

        if match:
            filename = match.group(2)
            extension = match.group(3)
            full_name = "{}{}".format(filename, extension).lower()
            
            if full_name not in seen_files:
                seen_files[full_name] = True
                print("{}{}".format(filename, extension))

                with open("{}_files.list".format(sys.argv[1]), "a") as f:
                    f.write("{}\n".format(full_name, payload))

script.on('message', on_message)
script.load()
sys.stdin.read()