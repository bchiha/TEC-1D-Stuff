

class Text2allophone

	attr_reader :cmu2sp0256File, :cmuDictFile

	def initialize
		@cmu2sp0256File = 'cmu2sp0.symbols'
		@cmuDictFile = 'cmudict-0.7b'
		@cmuDict = Hash.new	#word -> allophones
		@cmuRef = Hash.new #allophones -> {sp0256 allophone, hex value}
	end

	def loadCmuRefFile
		File.open(@cmu2sp0256File, "r") do |file|
			file.each_line do |line| 
				parts = line.strip.split(' ')
				@cmuRef[parts[0]] = {:text=>parts[1],:hex=>parts[2].hex}
			end
		end
	end
	
	def loadCmuDictFile
		File.open(@cmuDictFile, "r") do |file|
			file.each_line do |line| 
				if line[0] != ';' 
					parts = line.encode!('UTF-8', 'binary', invalid: :replace, undef: :replace, replace: '').strip.split(" ")
					@cmuDict[parts[0]] = parts.last(parts.size-1) #just get the rest
				end
			end
		end
	end

	def translate
		while line = gets
			badWord = []
			printf("TXT> ")
			line.upcase.split(' ').each do |word|
				if @cmuDict.include?(word)
					@cmuDict[word].each do |allophone|
						printf("%s ",@cmuRef[allophone][:text])
					end
					printf(". ")
				else
					badWord << word unless word == "EOF" || word == "PP"
				end
			end
			printf("\n000> ")
			count = 0
			line.upcase.split(' ').each do |word|
				if @cmuDict.include?(word)
					@cmuDict[word].each do |allophone|
						count += 1
						printf("%02X ",@cmuRef[allophone][:hex])
						printf("\n%03X> ",count) if count.modulo(8) == 0
					end
					count += 1
					printf("%02X ", 3) #word gap
					printf("\n%03X> ",count) if count.modulo(8) == 0
				else
					if word == "EOF"
						count += 1
						printf("%02X ", 255)
						printf("\n%03X> ",count) if count.modulo(8) == 0
					end
					if word == "PP"
						count += 1
						printf("%02X ", 4)
						printf("\n%03X> ",count) if count.modulo(8) == 0
					end
				end
			end
			if !badWord.empty?
				puts "\nWord(s) '#{badWord.join(",")}' not found in dictionary"
			end
			printf("\n\n")
		end
	end
	
end

t2a = Text2allophone.new

t2a.loadCmuDictFile
t2a.loadCmuRefFile

puts "Type in a sentence to convert...(EOF for FF, PP for 200ms Pause, C-d to exit)\n\n"

t2a.translate

