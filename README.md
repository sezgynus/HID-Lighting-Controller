# HID Lighting Controller

İki bağımsız adreslenebilir RGB LED dizisini USB HID LampArray üzerinden kontrol eden Arduino firmware’i. `Microsoft_HidForWindows` ile ana bilgisayardan alınan renkler, `Adafruit_NeoPixel` aracılığıyla LED’lere aktarılır.

## Özellikler

- İki ayrı `Microsoft_HidLampArray` nesnesi ve toplam **62 LED**.
- Her LED için konum, renk kanalları ve programlanabilirlik bilgisi.
- Yalnızca rengi değişen dizilerde `show()` çağrısı.
- Ana bilgisayar kontrolü dışındaki otonom modda LED’ler kapalıdır.

## Varsayılan yapılandırma

| Özellik | Dizi 1 | Dizi 2 |
|---|---|---|
| Veri pini | A0 | A3 |
| LED sayısı | 47 | 15 |
| Bildirilen boyutlar | 360 × 376 × 1 mm | 120 × 120 × 1 mm |
| Yerleşim | İki kenar boyunca | Halka biçiminde |

Her iki çıkış da `NEO_GRB + NEO_KHZ800` kullanır. LED özniteliklerinde güncelleme gecikmesi 4 ms, LampArray oluşturucularında güncelleme aralığı parametresi 33 ms olarak tanımlıdır.

## Gereksinimler

- Arduino IDE ve seçilen kartın çekirdeği.
- `Microsoft_HidForWindows` ve `Adafruit_NeoPixel` kütüphaneleri.
- HID kütüphanesinin desteklediği, USB aygıt olarak çalışabilen bir kart. Depoda kesin kart modeli veya sabitlenmiş kütüphane sürümü belirtilmiyor.
- GRB/800 kHz uyumlu adreslenebilir RGB LED’ler ve LED yüküne uygun güç kaynağı.
- USB HID LampArray desteği bulunan bir ana bilgisayar/aydınlatma uygulaması.

## Kurulum

1. `HID_Lighting_Controller/HID_Lighting_Controller.ino` dosyasını Arduino IDE ile açın.
2. İki kütüphaneyi kurun ve HID kütüphanesinin desteklediği kartı seçin.
3. Birinci dizinin DIN girişini A0’a, ikinci dizininkini A3’e bağlayın. Denetleyici ve LED güç kaynağının GND hatlarını ortaklayın; besleme ve lojik seviyesini kullanılan LED’lere göre düzenleyin.
4. Derleyip USB üzerinden yükleyin.
5. Kartı LampArray destekleyen ana bilgisayara bağlayın ve renk kontrolünü ana bilgisayardaki uygulamadan yapın.

Firmware seri port komutları veya bağımsız efekt menüsü sunmaz. Ana bilgisayar kontrolü olmadığında siyah renk kullanılması nedeniyle LED’lerin kapalı kalması beklenen davranıştır.

## Yerleşimi uyarlama

LED sayısını değiştirirken hem `LAMP_ARRAY1_COUNT` / `LAMP_ARRAY2_COUNT` sabitlerini hem de `lamp_attributes.h` içindeki ilgili diziyi birlikte güncelleyin. `static_assert` kontrolleri sayılar uyuşmadığında derlemeyi durdurur.

Her `LampAttributes` girdisi bir LED kimliği, X/Y/Z konumu (mm), gecikme, kullanım amacı ve kanal özelliklerini taşır. Fiziksel LED sırası ile öznitelik dizisinin sırası eşleşmelidir. Kasa boyutları değişiyorsa `Microsoft_HidLampArray` oluşturucularındaki ölçüleri de uyarlayın.

## Dosyalar

- `HID_Lighting_Controller/HID_Lighting_Controller.ino`: USB LampArray durumlarını okur ve NeoPixel çıkışlarını günceller.
- `HID_Lighting_Controller/lamp_attributes.h`: iki dizinin LED konumlarını ve HID özniteliklerini tanımlar.

## Sorun giderme

- **USB aydınlatma aygıtı görünmüyor:** kartın ve çekirdeğin HID kütüphanesiyle uyumunu, USB veri kablosunu ve ana bilgisayar desteğini kontrol edin.
- **LED’ler kapalı:** otonom mod varsayılan olarak siyahtır; ana bilgisayarın cihazın kontrolünü aldığını doğrulayın.
- **Renkler/konumlar yanlış:** GRB sırasını, LED sayısını, veri yönünü ve öznitelik sıralamasını karşılaştırın.
