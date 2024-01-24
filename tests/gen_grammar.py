import random


max_rule_len = 4
max_rules_cnt = 5
max_terminal_rule = 3
terminals = ["a", "b", "c"]
non_terminals = ["S", "P"]

rules = dict()

for i in range(random.randint(1, max_rules_cnt)):
    from_symbol = random.choice(non_terminals)
    rule_rhs = []
    for j in range(random.randint(0, max_rule_len)):
        rule_rhs.append(random.choice(terminals + non_terminals))

    if from_symbol not in rules:
        rules[from_symbol] = []

    rules[from_symbol].append(tuple(rule_rhs))

for from_symbol in non_terminals:
    rule_rhs = []
    for i in range(random.randint(0, max_terminal_rule)):
        rule_rhs.append(random.choice(terminals))

    if from_symbol not in rules:
        rules[from_symbol] = []

    rules[from_symbol].append(tuple(rule_rhs))


for from_symbol, rhss in rules.items():
    print(from_symbol, "-> ", end="")
    rhss = list(set(rhss))
    for rhs in rhss[:-1]:
        print(*rhs, end="|")

    if len(rhss) != 0:
        print(*rhss[-1])

print()
print(*terminals)
print()
print(*non_terminals)
print()
print(random.choice(non_terminals))
