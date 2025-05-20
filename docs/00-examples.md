# Examples

Here's some examples of what is being in the works.

This is mostly a planning document and is subject to change.

I will try shaping this document sort of like a dev-log with "talk as you go" style changelog, with newest updates at the top and oldest at the bottom.

## 2025-05-20
Currently plans of the language is starting.

I'm inspired partially or heavily by [Tsoding's Porth](https://gitlab.com/tsoding/porth) and there will be a lot of similarities. I'm trying to find my own ways of doing things, but due to the nature of stack based languages, there's only so much "new" operations you can come up with. Stuff like `dup`, `2dup`, `over`, `swap`, `2swap`, `drop`, etc. Very standard in stack based languages. You always have those, so the similarities will exist in that regard. Sure you could come up with names that are differen't but we're not here to host a halloween party. We're here to write code.

This is what I'm thinking of for a start. This is what we should start with.
```cstack
19 .
```

What this would do, reading left to right, is first push `19` onto the stack. Similar to `push 19` in x86_64 assembly. Secondly, `.` would pop the top most item, then log it onto the screen.

Secondly after that is done, we would need arithmetics. For a start, let's do.
```cstack
34 35 +
```
This would firth push `34`, then push `35`. Then we get the `+` which pops the top 2 values, `34` and `35`, and adds them, to get 69, and then pushes `69` back onto the stack.<br>
The act of doing `a b +` rather than `a + b` is called [Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) which is the act of putting the operator after the two digits. It comes from [Polish Notation](https://en.wikipedia.org/wiki/Polish_notation) which is the act of putting the operator before the two digits. We however use the Reverse because it makes the process really simple to implement.

After `69` is pushed to the stack, well in this example it would be useless because the process would end. Thus we do this instead:
```cstack
34 35 + .
```

This would do the same as before, except we end up with `69 .` which as seen above would end up printing `69` to the screen. Then the process would terminate.

Before all that though, let's start working on pushing numbers as well as printing.

Oh, and while this will be a compiled language by the end of it, let's focus on keeping it interpreted for now.


