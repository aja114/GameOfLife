import random
board_size = random.randint(20, 100)
min_alive = int(board_size**2/3)
max_alive = int(board_size**2)
num_alive = random.randint(min_alive, max_alive)
alive = []
for j in range(num_alive):
	r = str(random.randint(0,board_size-1))
	c =  str(random.randint(0,board_size-1))
	alive.append(" ".join([r, c]))
with open("config.txt", "w") as f:
	f.write(str(board_size)+"\n")
	f.write(str(num_alive)+"\n")
	f.write("\n".join(alive))
