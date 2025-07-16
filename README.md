I messed up git history... ignore the diff between commits b151ff6 (root) and babab5b (commit #2). Two commits got lost
in between the two due to a lot of rebasing to try to remove .Trash-100 and .cache after .gitignore's were added too
late. Anyhoo... latest commit has everything that is supposed to be there so yeah.

Setup `compile_commands.json`: 
```bash
cmake -S . -B build
ln -s build/compile_commands.json .
```
