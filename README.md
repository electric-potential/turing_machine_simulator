# turing_machine_simulator

The goal of this project was to design and implement a descriptive turing machine "language". The script acts as an interpreter going through a
"turing machine source code" file one character at a time to execute the underlying instructions. Once compiled, the script can be executed
in a terminal by passing in the "turing machine source code" filename and the initial tape as arguments. A typical example of a turing machine
is one that converts all 0's and 1's on the tape to a's and b's, this can be done in the language as follows:

<pre>
[
	state: accept
	accept: 1
	start: 0
]

[
	state: reject
	accept: 2
	start: 0
]

[
	state: start
	accept: 0
	start: 1
]

----- state declarations above, delta functions below -----

{
	state: start
	read:  
	write:  
	move: 1
	new state: accept
}

{
	state: start
	read: 0
	write: a
	move: 1
	new state: start
}

{
	state: start
	read: 1
	write: b
	move: 1
	new state: start
}
</pre>

More information regarding the syntax of the language and its structure can be found in <pre>turing_machine_file_format.txt</pre> along with the above example.
This project was designed on a Unix system and will sometimes display odd printing behaviour on Windows machines, namely, newlines not being formatted properly.
Some modern C compilers have features that can automatically convert newline characters between platforms, using one of these compilers is highly recommended
for use on Windows machines.

