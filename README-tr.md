# HID Lighting Controller

[English](README.md) | [Türkçe](README-tr.md)

**USB üzerinden iki bağımsız adreslenebilir RGB LED grubunu kontrol etmek için HID LampArray kullanan Arduino uyumlu firmware.**

Firmware, bilgisayara **iki ayrı HID LampArray aygıtı** sunar. Bilgisayardan gelen her LED'e özel RGB durumları, 47 LED'lik ve 15 LED'lik iki fiziksel NeoPixel çıkışına aktarılır. Toplam **62 LED ayrı ayrı adreslenebilir**.

USB HID Lighting and Illumination arayüzü için `Microsoft_HidForWindows`, fiziksel LED çıkışları için `Adafruit_NeoPixel` kullanılır.

```text
                 USB bilgisayar / aydınlatma yazılımı
                              │
                              │ USB HID LampArray
                              ▼
                 ┌──────────────────────────┐
                 │ USB destekli denetleyici │
                 │                          │
                 │ Microsoft_HidLampArray   │
                 │      #1       #2         │
                 │       │         │         │
                 │       ▼         ▼         │
                 │   RGB durum   RGB durum   │
                 │       │         │         │
                 │  NeoPixel    NeoPixel     │
                 └──────┬──────────┬────────┘
                        │          │
                       A0         A3
                        │          │
                        ▼          ▼
                    47 LED      15 LED
                   L biçimli    dairesel
                    yerleşim     yerleşim
```

## Projeye Genel Bakış

HID Lighting Controller, özel bir seri haberleşme veya üreticiye özgü RGB protokolü yerine standart HID LampArray üzerinden bilgisayardan LED'lere uzanan kontrol yolunu uygular.

Her fiziksel LED bilgisayara bir `LampAttributes` kaydıyla tanıtılır. Bu kayıt LED kimliğini, üç boyutlu konumunu, güncelleme gecikmesini, kullanım amacını, desteklenen renk kanallarını, yoğunluk kazancını, programlanabilirlik bilgisini ve tuş ilişkilendirmesini içerir.

Böylece bilgisayar LED'leri yalnızca sıralı bir şerit olarak değil, fiziksel konumları tanımlanmış ayrı ışık kaynakları olarak görebilir.

Firmware çalışma sırasında:

1. iki NeoPixel çıkışını başlatır;
2. iki fiziksel LED grubunu temizler;
3. başlangıçta otonom renk olan siyahı uygular;
4. HID kütüphanesinden LampArray 1'in güncel durumunu sürekli okur;
5. RGB değerlerini NeoPixel renklerine dönüştürür;
6. yalnızca rengi değişen LED'leri günceller;
7. ilgili dizide en az bir değişiklik varsa `show()` çağırır;
8. aynı işlemleri LampArray 2 için bağımsız olarak tekrarlar.

## Hızlı Bakış

| Alan | LampArray 1 | LampArray 2 |
|---|---:|---:|
| Fiziksel LED sayısı | 47 | 15 |
| Veri pini | A0 | A3 |
| Bilgisayara bildirilen boyut | 360 × 376 × 1 mm | 120 × 120 × 1 mm |
| Geometri | İki kenarlı / L biçimli | Dairesel |
| LED kimlikleri | `0x00`–`0x2E` | `0x00`–`0x0E` |
| LED kullanım amacı | Vurgu aydınlatması | Vurgu aydınlatması |
| LED başına güncelleme gecikmesi | 4 ms | 4 ms |
| LampArray minimum güncelleme aralığı | 33 ms | 33 ms |
| Programlanabilir | Evet | Evet |
| RGB mantıksal maksimumu | 255 / 255 / 255 | 255 / 255 / 255 |
| Yoğunluk kazancı | 1 | 1 |
| NeoPixel biçimi | GRB, 800 kHz | GRB, 800 kHz |

Toplam fiziksel LED sayısı: **62**.

## HID LampArray Mimarisi

İki bağımsız `Microsoft_HidLampArray` nesnesi oluşturulur:

```cpp
Microsoft_HidLampArray lampArray1 = Microsoft_HidLampArray(
    47, 360, 376, 1,
    LampArrayKindPeripheral,
    33,
    LampAttributes1
);

Microsoft_HidLampArray lampArray2 = Microsoft_HidLampArray(
    15, 120, 120, 1,
    LampArrayKindPeripheral,
    33,
    LampAttributes2
);
```

İki dizi tek bir 62 LED'lik koordinat sistemi altında birleştirilmez. Her dizinin kendi boyutları, öznitelik tablosu, HID durumu ve NeoPixel çıkışı vardır.

İkisi de `LampArrayKindPeripheral` olarak tanımlanmıştır.

## Fiziksel LED Çıkışları

Firmware iki bağımsız `Adafruit_NeoPixel` nesnesi oluşturur:

| Çıkış | Pin | LED sayısı | Biçim |
|---|---:|---:|---|
| `ledStrip1` | A0 | 47 | `NEO_GRB + NEO_KHZ800` |
| `ledStrip2` | A3 | 15 | `NEO_GRB + NEO_KHZ800` |

Kod, 800 kHz NeoPixel tipi haberleşmeyle ve GRB kanal sıralamasıyla uyumlu LED'ler varsayar.

Depoda sinyal pinleri ve mantıksal LED topolojisi tanımlıdır; ancak LED besleme gerilimi, güç dağıtımı, seviye dönüştürme, kullanılan denetleyici kart revizyonu veya maksimum akım bütçesi belgelenmemiştir. Bu elektriksel ayrıntılar firmware'den varsayılmak yerine kullanılan gerçek LED donanımına göre belirlenmelidir.

## LED Geometrisi

### LampArray 1 — 47 LED

Birinci dizi L biçimli, iki kenar boyunca ilerleyen bir yerleşimi tanımlar.

`0x00`–`0x16` kimlikleri Y sıfırda sabitken X ekseni boyunca ilerler:

```text
(0,0) → (16,0) → (32,0) → ... → (352,0)
```

Kalan LED'ler köşeyi döner ve X = 360 konumunda Y ekseni boyunca ilerler:

```text
(360,8)
(360,24)
(360,40)
   ...
(360,376)
```

Şematik görünüm:

```text
0x00 ─ 0x01 ─ 0x02 ─ ... ─ 0x16
                              │
                            0x17
                              │
                            0x18
                              │
                              ⋮
                              │
                            0x2E
```

`Microsoft_HidLampArray` boyutları **360 × 376 × 1 mm** olarak bildirilir.

### LampArray 2 — 15 LED

İkinci dizi, yaklaşık 120 × 120 mm'lik dairesel alan çevresine dağıtılmış 15 konum tanımlar.

Koordinatlar sağ taraftaki `(116, 51)` noktasından başlar; alt yarı, sol taraf ve üst yarı boyunca saat yönünde ilerleyerek `(108, 30)` civarında tamamlanır.

Şematik görünüm:

```text
             0x0B  0x0C
        0x0A             0x0D
    0x09                     0x0E
 0x08                           0x00
 0x07                           0x01
    0x06                     0x02
        0x05             0x03
              0x04
```

Gerçek konumlar `lamp_attributes.h` içindeki tam sayı koordinatlarıdır; yukarıdaki çizim yalnızca sıralamayı görselleştirir.

## LED Öznitelikleri

Kimlik ve konum dışında bütün LED kayıtları aynı yetenek bilgilerine sahiptir:

| Öznitelik | Değer |
|---|---|
| Z koordinatı | 0 |
| Güncelleme gecikmesi | 4 ms |
| Kullanım amacı | `LampPurposeAccent` |
| Kırmızı mantıksal maksimum | `0xFF` |
| Yeşil mantıksal maksimum | `0xFF` |
| Mavi mantıksal maksimum | `0xFF` |
| Yoğunluk kazancı | `0x01` |
| Programlanabilirlik | `LAMP_IS_PROGRAMMABLE` |
| Giriş tuşu ilişkilendirmesi | `0x00` |

Kaynak koddaki açıklamaya göre konumlar cihazın sol üst köşesinden itibaren milimetre cinsinden tanımlanmıştır.

## Çalışma Zamanı Veri Akışı

Her LampArray için ana döngü aynı işlemleri yapar:

```text
Microsoft_HidLampArray
        │
        │ getCurrentState()
        ▼
LampArrayColor[N]
        │
        │ otonom mod?
        ├──────────── evet ──► siyah
        │
        └──────────── hayır
                        │
                        ▼
              Kırmızı / Yeşil / Mavi
                        │
                        ▼
            NeoPixel paketlenmiş renk
                        │
                        ▼
             mevcut LED rengiyle karşılaştır
                        │
              değişti? ─┴── hayır → atla
                 │
                evet
                 ▼
          setPixelColor()
                 │
                 ▼
          en az bir değişiklik?
                 │
                evet
                 ▼
               show()
```

Bu değişiklik kontrolü, dizideki bütün LED'ler zaten istenen renkteyse gereksiz `show()` çağrısını engeller.

İki dizi sırayla işlenir ve ayrı `update` bayrakları kullanır.

## Otonom Mod

Firmware şu rengi tanımlar:

```cpp
uint32_t lampArrayAutonomousColor = ledStrip1.Color(0, 0, 0);
```

`getCurrentState()` ilgili LampArray'in otonom modda olduğunu bildirdiğinde fiziksel LED'lere bilgisayardan gelen renkler yerine bu renk uygulanır.

Otonom renk şu anda **RGB(0, 0, 0)** olduğundan otonom modun fiziksel sonucu **LED'lerin kapalı olmasıdır**.

Depoda bağımsız animasyon motoru, efekt seçici, seri port komut arayüzü veya yerel renk kontrol arayüzü bulunmaz.

## Renk Dönüşümü

Bilgisayardan gelen renkler `LampArrayColor` olarak tutulur. Firmware bunları şu şekilde dönüştürür:

```cpp
return ledStrip1.Color(
    lampArrayColor.RedChannel,
    lampArrayColor.GreenChannel,
    lampArrayColor.BlueChannel
);
```

Yardımcı fonksiyon iki dizi için de `ledStrip1.Color()` kullansa da bu çağrı yalnızca verilen RGB bileşenlerini NeoPixel kütüphanesinin paketlenmiş renk değerine dönüştürür. Oluşan değer daha sonra iki fiziksel diziden herhangi birinde kullanılabilir.

Fiziksel NeoPixel nesneleri **GRB** kanal sırasına göre yapılandırılmıştır.

## Güncelleme Zamanlaması

Kaynak kodda birbirinden farklı iki zaman değeri vardır:

- `NEO_PIXEL_LAMP_UPDATE_LATENCY = 0x04` → her LED'in özniteliklerinde bildirilen **4 ms**.
- LampArray constructor güncelleme aralığı → her HID LampArray için **33 ms**.

Bu değerler farklı katmanları tanımlar ve birbirinin yerine kullanılmamalıdır.

Ana Arduino döngüsünde açık bir `delay()` çağrısı bulunmaz.

## Başlangıç Sırası

`setup()` iki diziyi bağımsız olarak başlatır:

```text
ledStrip1.begin()
ledStrip1.clear()
ledStrip1.fill(siyah, 0, 46)
ledStrip1.show()

ledStrip2.begin()
ledStrip2.clear()
ledStrip2.fill(siyah, 0, 14)
ledStrip2.show()
```

Böylece normal bilgisayar durumlarını işleme başlamadan önce iki çıkış da yapılandırılmış otonom renkle başlar.

## Derleme Zamanı Tutarlılık Kontrolleri

Firmware, fiziksel LED sayılarının ilgili öznitelik tablolarıyla eşleşmesini derleme sırasında doğrular:

```cpp
static_assert(
    sizeof(LampAttributes1) / sizeof(LampAttributes)
        == LAMP_ARRAY1_COUNT
);

static_assert(
    sizeof(LampAttributes2) / sizeof(LampAttributes)
        == LAMP_ARRAY2_COUNT
);
```

LED sayısı değiştirilip karşılık gelen `LampAttributes` dizisi güncellenmezse firmware tutarsız HID bilgileriyle derlenmek yerine derleme hatası verir.

## Yerleşimi Uyarlama

Firmware başka bir LED düzenine uyarlanırken dört katmanın birlikte güncellenmesi gerekir:

1. **Fiziksel LED sayısı** — `LAMP_ARRAY1_COUNT` / `LAMP_ARRAY2_COUNT`.
2. **Çıkış bağlantısı** — `NEO_PIXEL1_PIN` / `NEO_PIXEL2_PIN`.
3. **HID geometrisi** — `LampAttributes1` / `LampAttributes2` içindeki bütün kayıtlar.
4. **Bildirilen fiziksel boyutlar** — her `Microsoft_HidLampArray` oluşturucusuna verilen genişlik, yükseklik ve derinlik.

Her öznitelik dizisinin sırası fiziksel adreslenebilir LED sırasıyla eşleşmelidir. Bilgisayar, bildirilen koordinatları her LED'in fiziksel kurulumdaki yerini anlamak için kullanır.

LED sayısı değiştirildiğinde hem count sabiti hem de ilgili öznitelik tablosu güncellenmelidir; derleme zamanı kontrolleri sayı uyuşmazlığını yakalar.

## Gereksinimler

Kaynak kod doğrudan şunlara bağımlıdır:

- `Microsoft_HidForWindows`;
- `Adafruit_NeoPixel`;
- HID kütüphanesinin desteklediği, USB aygıtı olarak çalışabilen Arduino uyumlu bir hedef;
- seçilen NeoPixel zamanlaması ve kanal sırasıyla uyumlu iki adreslenebilir RGB LED zinciri;
- HID LampArray aygıtlarını kontrol edebilen bir bilgisayar ortamı/uygulaması.

Depoda şu sürümler sabitlenmemiştir:

- kesin kart tanımı;
- Arduino core sürümü;
- `Microsoft_HidForWindows` sürümü;
- `Adafruit_NeoPixel` sürümü.

Firmware doğal USB HID desteği üzerine kurulmuştur. Yalnızca USB-seri dönüştürücü üzerinden haberleşen kartlar, farklı donanım/USB desteği olmadan aynı HID aygıt davranışını sağlayamaz.

## Derleme ve Kurulum

1. Arduino geliştirme ortamında `HID_Lighting_Controller/HID_Lighting_Controller.ino` dosyasını açın.
2. `Microsoft_HidForWindows` ve `Adafruit_NeoPixel` kütüphanelerini kurun.
3. Uyumlu, doğal USB destekli kart/core seçin.
4. Birinci LED grubunun veri girişini A0'a, ikinci grubunkini A3'e bağlayın.
5. LED donanımına uygun elektriksel beslemeyi sağlayın ve donanımın gerektirdiği şekilde denetleyiciyle ortak referans oluşturun.
6. Firmware'i derleyip karta yükleyin.
7. Denetleyiciyi USB üzerinden bilgisayara bağlayın.
8. LED'lerin kontrolünü almak için HID LampArray destekli bir bilgisayar uygulaması kullanın.

## Sorun Giderme

### HID aydınlatma aygıtı görünmüyor

Seçilen kart/core'un doğal USB HID desteğini ve `Microsoft_HidForWindows` uyumluluğunu kontrol edin. USB kablosunun yalnızca güç değil veri de taşıdığını doğrulayın.

### LED'ler kapalı kalıyor

Otonom renk siyah olarak yapılandırılmıştır. Bilgisayarın LampArray kontrolünü aldığını ve sıfırdan farklı LED renkleri gönderdiğini doğrulayın.

### Renkler yanlış

Fiziksel çıkışlar `NEO_GRB + NEO_KHZ800` olarak yapılandırılmıştır. Kullanılan LED'lerin aynı kanal sırasını ve zamanlamayı kullandığını doğrulayın.

### Konuma bağlı efektler yanlış LED'lerde görünüyor

Fiziksel LED sırasını ilgili `LampAttributes` tablosuyla karşılaştırın. HID koordinatları mantıksal konumu tanımlarken dizi sırası hangi fiziksel LED'in hangi durumu alacağını belirler.

### LED sayısını değiştirdikten sonra derleme başarısız

İlgili `LampAttributes` dizisini de güncelleyin. `static_assert` kontrolleri sayı uyuşmazlıklarını bilerek reddeder.

## Kaynak Dosya Haritası

| Dosya | Sorumluluk |
|---|---|
| `HID_Lighting_Controller/HID_Lighting_Controller.ino` | İki HID LampArray ve NeoPixel çıkışını oluşturur, LED'leri başlatır, bilgisayar durumunu okur, renk dönüşümünü yapar ve değişen LED'leri günceller |
| `HID_Lighting_Controller/lamp_attributes.h` | 62 LED'in kimliklerini, fiziksel koordinatlarını ve HID yeteneklerini tanımlar |
| `.github/workflows/sign-commits.yml` | Deponun commit imzalama workflow'u |

## Depo Yapısı

```text
HID-Lighting-Controller/
├── .github/
│   └── workflows/
│       └── sign-commits.yml
├── HID_Lighting_Controller/
│   ├── HID_Lighting_Controller.ino
│   └── lamp_attributes.h
├── README.md
└── README-tr.md
```

## Mevcut Kapsam

Depo, **USB HID LampArray durumu → iki fiziksel NeoPixel dizisi** arasındaki firmware yolunu uygular.

Şu anda donanım şeması, PCB dosyaları, masaüstü kontrol uygulaması, özel USB VID/PID yapılandırması, bağımsız efekt motoru veya sabitlenmiş derleme ortamı bilgisi içermez. Bu nedenle dokümantasyonda bu unsurlara ilişkin varsayım yapılmamıştır.
