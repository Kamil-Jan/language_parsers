import random


terminals = ["a", "b", "c"]
max_length = 5

word = [];
for i in range(random.randint(0, max_length)):
    word.append(random.choice(terminals))

print(*word)
