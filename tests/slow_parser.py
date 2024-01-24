import queue


class Grammar:
    def __init__(self, filename):
        self.rules = dict()
        self.terminals = set()
        self.non_terminals = set()
        self.start_non_terminal = ""

        cnt = 0
        with open(filename, "r") as file:
            for line in file:
                line = line.strip()
                if not line:
                    cnt += 1
                    continue

                if cnt == 0:
                    lhs, rhss = line.split("->")
                    lhs = lhs.strip()
                    for rhs in rhss.split("|"):
                        rhs = rhs.strip()
                        rhs_list = rhs.split(" ")
                        if lhs not in self.rules:
                            self.rules[lhs] = []
                        self.rules[lhs].append(rhs_list)
                elif cnt == 1:
                    self.terminals = line.split(" ")
                elif cnt == 2:
                    self.non_terminals = line.split(" ")
                elif cnt == 3:
                    self.start_non_terminal = line

    def get_terminals_string(self, now):
        cur = []
        for symbol in now:
            if symbol not in self.non_terminals and symbol:
                cur.append(symbol)
        return tuple(cur)

    def parse(self, word: tuple[str]) -> bool:
        q = queue.Queue()
        q.put([self.start_non_terminal])
        used = {(self.start_non_terminal)}
        iter = 0
        max_iter = 5_000
        while not q.empty():
            if iter == max_iter:
                break
            iter += 1
            now = q.get()
            terminals_string = self.get_terminals_string(now)
            if terminals_string == now:
                if terminals_string == word:
                    return True
                continue

            if len(terminals_string) > len(word):
                continue

            for i in range(len(now)):
                if now[i] not in self.non_terminals:
                    continue
                for rhs in self.rules[now[i]]:
                    if len(rhs) == 1 and not rhs[0]:
                        cur = list(now[:i]) + list(now[i + 1:])
                    else:
                        cur = list(now[:i]) + rhs + list(now[i + 1:])
                    cur = tuple(cur)
                    if cur not in used:
                        used.add(cur)
                        q.put(cur)
        return False

def main():
    grammar = Grammar("grammar_file")
    word = ""
    with open("word_file") as file:
        word = file.readline()
        word = word.strip()

    print(int(grammar.parse(tuple(word.split()))))


if __name__ == "__main__":
    main()
