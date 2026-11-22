# Synapse

Synapse is a scripting language designed to simplify Command-Line Interface operations.

It acts as an interface between users and executable programs, translating command-line arguments into JSON payloads.

Synapse launches the target executable as a child process and transfers the payload through stdin. Once the target validates the payload and acknowledges successful receipt (via c++ library), Synapse detaches the target process and terminates itself, allowing the target to continue execution independently.

(Note that Synapse sends JSON payloads synchronously through a single stdin pipe. So it is limited to only one JSON payload at a time)


## Example

A simple Synapse script (`main.syn`):

```syn
#this is a comment in Synapse
target "../bin/app"


option "help" {
    print "Welcome to Synapse Scripting Language!"

    flag "-short" {
        #Default delimiter is space!
        print "Welcome! \n"
    }

    flag "--name",delimiter="=",argcount=1{
        print "Welcome, "
        print arg:0
        print "! \n"
    }
}

option "send"{
    flag "-text",argcount=1{
        addToPayload "received",arg:0
    }
}
```

Run the script using:

```bash
synapse help -short
```

### Custom script location

By default, Synapse looks for `main.syn` alongside its executable. If your script has a different filename or is stored elsewhere, you can specify its location using `--custompath` (Note that the path is relative to the Synapse executeable):

```bash
synapse --custompath="../files/synapse.syn" help -short
```

To cleanly use in projects rename/recompile the executeable to your project name.