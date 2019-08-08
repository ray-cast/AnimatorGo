import os,argparse,sys,string

#
# Simple helper program - give it a path and it will list all of the files of the specified extension in relative format, using the 
# pathroot variable as a substitution. This greatly simplifies one part of the process of creating a CMake file for a project
#
parser = argparse.ArgumentParser()
parser.add_argument("dir", help="Path to find files in")
parser.add_argument("extension", help="Spec to find (ie .cpp)")
parser.add_argument("--pathroot", help="Path variable to prepend to each line, example: ${PX_ROOT}", default="${DUDER}")

args = parser.parse_args()

if not os.path.exists(args.dir):
	print("Unable to find path {}".format(args.dir))
	exit(1)

for root, dirs, files in os.walk(args.dir):
	for file in files:
		if file.endswith(args.extension):
			result = os.path.join(root, file)
			
			print(result.replace(args.dir, args.pathroot))
