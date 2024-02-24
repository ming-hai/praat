writeInfoLine: "Testing fileReadable()"
assert fileReadable: "../runAllTests.praat"
assert fileReadable: "fileReadable.praat"
assert not fileReadable: "lguianeryv;ikneir;rhuvei.praat"
assert not fileReadable: "lguianeryfgdfghdshdfghhuvei.praat"
assert not fileReadable: "../sys"
assert fileReadable: "../sys/graphics.praat"
assert not fileReadable: "up"
assert fileReadable: "up/fileNames-S-H-up.praat"

assert folderExists: "."
assert folderExists: "./"
assert folderExists: ".."
assert folderExists: "../"
assert folderExists: "up"
assert folderExists: "up/"
assert folderExists: "../speed"
assert folderExists: "../speed/"
assert not folderExists: "../speedometers"
assert not folderExists: "hffghfdjkgh"
assert not folderExists: "hffghfdjkgh/"
assert not folderExists: "up/hffghfdjkgh"
assert folderExists: "~"
assert folderExists: "~/"
assert not folderExists: "~/hfglkygghjglj"

appendInfoLine: "OK"