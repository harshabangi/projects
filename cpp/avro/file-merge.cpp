#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "avro/Specific.hh"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>

class Header {
   public:
      std::vector<uint8_t> sync;
      std::map<std::string, std::vector<uint8_t> > meta;
};

class Merge {
   public:
      avro::OutputStreamPtr outPtr;
      avro::EncoderPtr encoder;
      bool empty = true;
      std::vector<uint8_t> sync;

      Merge(const char* path) {
         outPtr = avro::fileOutputStream(path);
         encoder = avro::binaryEncoder();
         encoder->init(*outPtr);
      }

      Header loadHeader(avro::DecoderPtr decoder) {
         Header h = Header();
         std::vector<uint8_t> magic(4);
         try {
            decoder->decodeFixed(4);
         } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
         }
         long l = decoder->mapStart();
         if (l > 0) {
            do {
               for (long i = 0 ; i < l ; i++) {
                  std::string key = decoder->decodeString();
                  h.meta[key] = decoder->decodeBytes();
               }
            } while ((l = decoder->mapNext()) != 0);
         }
         h.sync = decoder->decodeFixed(16);
         return h;
      }

      void saveHeader(avro::EncoderPtr encoder, Header header) {
         if (empty) {
            encoder->mapStart();
            encoder->setItemCount(header.meta.size());
            for (auto const[k, v]: header.meta) {
               encoder->startItem();
               encoder->encodeString(k);
               encoder->encodeBytes(v);
            }
            encoder->mapEnd();
            encoder->encodeFixed(header.sync);
            sync = header.sync;
            empty = false;
         }
      }

      void add(avro::InputStreamPtr inPtr, std::ifstream &ifs) {
         avro::DecoderPtr decoder = avro::binaryDecoder();
         decoder->init(*inPtr);
         Header header = loadHeader(decoder);
         saveHeader(encoder, header);
         while(!ifs.eof()) {
            int64_t items = decoder->decodeLong();
            size_t bytes = (size_t) decoder->decodeLong();
            std::cout << "items: " << items << " bytes: " << bytes << "\n";
            std::vector<uint8_t> buffer = decoder->decodeFixed(bytes);
            std::vector<uint8_t> sync = decoder->decodeFixed(16);
            encoder->encodeLong(items);
            encoder->encodeLong(bytes);
            encoder->encodeFixed(buffer);
            encoder->encodeFixed(sync);
         }
      }
};

void print(std::vector<uint8_t> arr) {
   for (int i = 0 ; i < arr.size() ; i++) {
      printf("%c", arr[i]);
   }
}

int main(int argc, char *argv[]) {
   Merge m = Merge("/Users/harshavardhan/kubernetes-logs/1.avro");
   //std::ifstream ifs("cpx.json");
   //avro::InputStreamPtr in = avro::istreamInputStream(ifs);
   //ifs.eof();
    std::ifstream ifs("/Users/harshavardhan/kubernetes-logs/part-00000.avro");
    m.add(avro::istreamInputStream(ifs, 4), ifs);
    ifs.close();
   //m.add(avro::fileInputStream("/Users/harshavardhan/kubernetes-logs/part-00000.avro"));
   return 0;
}