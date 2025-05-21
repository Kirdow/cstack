# Examples

Here's some examples of what is being in the works.

This is mostly a planning document and is subject to change.

I will try shaping this document sort of like a dev-log with "talk as you go" style changelog, with newest updates at the top and oldest at the bottom.

## 2025-05-21 10:10
`else` is working, and I also took the time to implement hashmaps. Hashmaps are currently strings only, but it's a basis for the next feature. Macros.

Macros are snippets of code, which have a name and a body. This creates two phases for parsing. First we scan for macros and store them in a hashmap.<br>
We subsequently strip the final code vector from these macros.<br>
We then iterate recursively (with a boundary) over the code vector and replaces every occurrence of a macro name with its body.<br>
We do this until we no longer find any more macro references, in which we start executing the program itself, or until we reach an expansion limit, in which we throw an error and exit.<br>

At the end of implementing macros, we should be able to write this code:
```cstack
macro sum.
    2dup + .
end
34 35 sum.
. .
```

and have it first evaluate into this pair:
```
# hashmap:
sum. => 2dup + .

# code:
34 35 sum. . .
```
and after expanding each macro name, the code should become:
```cstack
34 35 2dup + . . .
```
which should in turn output:
```
69
35
34
```

## 2025-05-20 21:52
If statement (or If conditions) are now working. The example we used though, `06-if.cst`, has a few rough edges. Namely this:
```cstack
1 if
    70 .
end

0 if
    68 .
end
```

Wouldn't it be easier if we just had it be this:
```cstack
1 if
    70 .
else
    68 .
end
```

Now to think about this. What happens when we encounter `if`? Well if the condition is `true`, we enter the if statement. If it's `false`, we jump to `end`, except in cases like these where we got `else`, we should jump there instead.

Likewise, if we encounter `else`, we know that we just ran the `if` body, meaning `else` shouldn't run, and instead we jump to `end` from here.

This means, for our current implementation, `end` should stay the same, except it should work the same for `else` as it does for `if` currently. Additionally, when we encounter `end`, we should do to `if` what `end` already does, by giving `if` a `next` value of the `pointer` after `else`. Additionally, we should also push back the current IP because `end` would need to know where `else` is located in order to provide the skip to `end` that `else` needs.

Given this, at the end of the current task, we should have this code:
```cstack
1 if
    70 .
    1 if
        1 .
    end
else
    68 .
    1 if
        2 .
    end
end

0 if
    421 .
    1 if
        3 .
    end
else
    419 .
    1 if
        4 .
    end
end
```

This should output:
```
70
1
419
4
```

## 2025-05-20 21:03
Time for something new. If statements. This introduces a new problem. Control flow. Currently, all we do after an instruction is to jump to the next instruction pointer by incrementing `program->ip` by one.

We need to introduce a new component to the program. `instruction_t`. This one should have the following structure:
```c
struct instruction_t {
    struct token_t *token;
    ssize_t next;
}
```

`token` would be the token we currently store in the `program->program_code` vector, now shifted into the `instruction_t` struct.<br>
`next` would be an indicator to the control flow what to do at the end of a an instruction depending on its outcome. If `next` is -1, we default to incrementing `program->ip` by one. However, if it is not -1, it gets more contextual.

For starters, with `if`, the way it works is that it pops the top most value. If it's non-zero (truthy), just increment IP by one as normal and enter the body. However, if the value is zero (falsy), we want to jump to the `end` keyword.

Now how do we know the location of `end` before we discover it? simply: we don't. This is why to generate the `instruction_t` vector from `token_t` vector we need a pre-processor. The pre-processor would be `O(n)` where `n` is the amount of tokens in the program.<br>
When it first discovers `if`, push the presence of this keyword by pushing it's pointer to a secondary stack. When we discover `end`, we then pop from this secondary stack, and modify the `if` value at that pointer to have its `next` variable set to the newly discovered `end` pointer.<br>
However, this creates a problem. When we implement While loops, stumbling upon `end` from within the loop should obviously jump back to the condition. This is why bindings towards an `end` statement, shouldn't jump to the pointer for `end` but to whatever is located after `end`.<br>
This is obviously context dependent. For `if` in particular, `end` should also just default to incrementing by 1, as it wouldn't make sense to run an if-statement twice. This should resolve any question about nested if-statements where you may encounter `end` being written twice in a row. Should `end` look ahead for duplicate `end`s? No. It should not.

So by the end of this task, we should have this code:
```cstack
0 if
    420 .
    1 if
        421 .
    end

    0 if
        419 .
    end
end

1 if
    69 .

    1 if
        70 .
    end

    0 if
        68 .
    end
end
```

This should output
```
69
70
```

## 2025-05-20 20:42
Lastly, we've forgotten the `drop` operator. This one purely discards the top most stack value. It's identical to the following:
```x86asm
add rsp, 8
```

This means after implementing `drop`, we should be able to try a simple program
```cstack
0 35 34 2dup +
2swap swap drop
swap . + .
```

This should print out the following:
```
69
69
```

And the stack should also be empty as indicated by a clean exit.

## 2025-05-20 20:22
Now it's time for `2dup` and `2swap`. `2dup` could be made as a macro `over over`. However having it natively seems crucial. Same goes for `2swap` when we got a return stack, which we don't yet, so we're adding that as well.

`2dup` therefore would be the following in x86_64:
```x86asm
mov rax, [rsp+8]
mov rbx, [rsp]
push rax
push rbx
```

`2swap` on the other hand requires some more direct approach:
```x86asm
pop rax
pop rbx
pop rcx
pop rdx
push rbx
push rax
push rdx
push rcx
```

Notice the order of `rax` and `rbx` stays the same, likewise `rcx` and `rdx` order stays. However, we swap the `rax`/`rbx` pair with the `rcx`/`rdx` pair.

This means by the end of this session, we should be able to run the following code:
```cstack
34 35 5 7 2swap + 0 swap 2swap +
2dup + 2swap +
. . .
```

12 81 69
which should then output
```
69
81
12
```

## 2025-05-20 19:27
Now that we got `dup` and `swap` we also need `over` which I forgot to mention earlier. It's the basis for `2dup` and `2swap`.

`over` should simply copy the 2nd top most value, and push it onto stack.

in x86_64 assembly this could look something like this:
```x86asm
pop rax
pop rbx
push rbx
push rax
push rbx
```

or you could do it like this:
```x86asm
mov rax, [rsp+8]
push rax
```

So after `over` is implemented, we should be able to test this code:
```cstack
34 35 over . + .
```
This should yield the output
```cstack
34
69
```

## 2025-05-20 19:18
Now the basic arithmetics operators have been implemented.

Next up is basic keywords. Time to do `dup` and `swap`.

`dup` should take the top value on the stack, and push it back twice.<br>
`swap` should take the top two values on the stack, and push them back in reverse order.

At the end of the session, this demo:
```cstack
7 76 dup .
swap - .
```
Should have the following output:
```
76
69
```

## 2025-05-20 18:39
Ops `+` and `.` has been implemented. Now it's time to finish all the arithmetics operators.

Next ups is `-`, then `*`, `/` and `%`. We need to determine what happens on division by zero. For now I'm gonna make it error gracefully by making `0 /` and `0 %` provide a result of `0`. In the future when we have `if` statements, the programmer would be able to check for zero divisions on their own to account for this. In a way this makes the language "unsafe", but I don't really want to deal with error handling atm with a language so fresh.

By the end of this session, these should all print 690
```cstack
340 350 + .
1050 360 - .
23 30 * .
6210 9 / .
9690 1000 % .
```
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


