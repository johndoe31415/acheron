# acheron
This is a project purely archived for historical purposes: acheron is a
user-space firewall for Linux which my awesome friend Severin Strobl
([@severinstrobl](https://github.com/severinstrobl)) and I hacked for a
university project
(["Blockpraktikum System-und Netzwerksicherheit 2"](https://www4.cs.fau.de/Lehre/WS07/PR_SNS/),
i.e., Systems- and Network Security) in a week. It was challening and fun and
we decided to push it to Github to remember the good memories of kernel
oooopses and cursing. We did this when we were in 9th semester of computer
science at University of Erlangen.

It has a kernel portion which sends connection requests to userspace for
userspace to determine what it should do with it. If I'm not entirely mistaken
this is similar to how applications like apparmor work, too.

Don't expect this to compile. Absoutely do not expect it to work.

There's a
[presentation](https://github.com/johndoe31415/acheron/raw/master/vortrag/Vortrag.pdf)
we did for the project as well.

## Naming
Because the exercise was fairly painful, we named the project after
[Acheron](https://en.wikipedia.org/wiki/Acheron), a river often referenced in
Greek mythology as the "river of woe".

## License
CC-0.
