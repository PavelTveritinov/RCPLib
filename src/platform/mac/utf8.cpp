#include "platform/utf8.h"

#include <CoreFoundation/CoreFoundation.h>

#include <cerrno>
#include <stdexcept>
#include <string_view>
#include <string>
#include <vector>

#include "scoped_cftyperef.h"

namespace base {

    namespace {

        // Convert the supplied CFString into the specified encoding, and return it as
        // an STL string of the template type.  Returns an empty string on failure.
        //
        // Do not assert in this function since it is used by the asssertion code!
        template<typename StringType>
        static StringType CFStringToSTLStringWithEncodingT(CFStringRef cfstring,
                                                           CFStringEncoding encoding) {
            CFIndex length = CFStringGetLength(cfstring);
            if (length == 0)
                return StringType();

            CFRange whole_string = CFRangeMake(0, length);
            CFIndex out_size;
            CFIndex converted = CFStringGetBytes(cfstring,
                                                 whole_string,
                                                 encoding,
                                                 0,      // lossByte
                                                 false,  // isExternalRepresentation
                                                 NULL,   // buffer
                                                 0,      // maxBufLen
                                                 &out_size);
            if (converted == 0 || out_size == 0)
                return StringType();

            // out_size is the number of UInt8-sized units needed in the destination.
            // A buffer allocated as UInt8 units might not be properly aligned to
            // contain elements of StringType::value_type.  Use a container for the
            // proper value_type, and convert out_size by figuring the number of
            // value_type elements per UInt8.  Leave room for a NUL terminator.
            typename StringType::size_type elements =
            out_size * sizeof(UInt8) / sizeof(typename StringType::value_type) + 1;

            std::vector<typename StringType::value_type> out_buffer(elements);
            converted = CFStringGetBytes(cfstring,
                                         whole_string,
                                         encoding,
                                         0,      // lossByte
                                         false,  // isExternalRepresentation
                                         reinterpret_cast<UInt8*>(&out_buffer[0]),
                                         out_size,
                                         NULL);  // usedBufLen
            if (converted == 0)
                return StringType();

            out_buffer[elements - 1] = '\0';
            return StringType(&out_buffer[0], elements - 1);
        }

        // Given an STL string |in| with an encoding specified by |in_encoding|,
        // convert it to |out_encoding| and return it as an STL string of the
        // |OutStringType| template type.  Returns an empty string on failure.
        //
        // Do not assert in this function since it is used by the asssertion code!
        template<typename InStringType, typename OutStringType>
        static OutStringType STLStringToSTLStringWithEncodingsT(
                                                                const InStringType& in,
                                                                CFStringEncoding in_encoding,
                                                                CFStringEncoding out_encoding) {
            typename InStringType::size_type in_length = in.length();
            if (in_length == 0)
                return OutStringType();

            base::ScopedCFTypeRef<CFStringRef> cfstring(CFStringCreateWithBytesNoCopy(
                                                                                      NULL,
                                                                                      reinterpret_cast<const UInt8*>(in.data()),
                                                                                      in_length * sizeof(typename InStringType::value_type),
                                                                                      in_encoding,
                                                                                      false,
                                                                                      kCFAllocatorNull));
            if (!cfstring)
                return OutStringType();

            return CFStringToSTLStringWithEncodingT<OutStringType>(cfstring,
                                                                   out_encoding);
        }

        // Specify the byte ordering explicitly, otherwise CFString will be confused
        // when strings don't carry BOMs, as they typically won't.
        static const CFStringEncoding kNarrowStringEncoding = kCFStringEncodingUTF8;
#ifdef __BIG_ENDIAN__
        [[maybe_unused]] static const CFStringEncoding kWideStringEncoding = kCFStringEncodingUTF32BE;
#elif defined(__LITTLE_ENDIAN__)
        [[maybe_unused]] static const CFStringEncoding kWideStringEncoding = kCFStringEncodingUTF32LE;
#else
#  error Either __BIG_ENDIAN__ or __LITTLE_ENDIAN__ must be defined.
#endif  // __LITTLE_ENDIAN__
    }  // namespace
}  // namespace base

namespace Platform
{
    std::string toUtf8( const std::wstring_view& input )
    {
        return base::STLStringToSTLStringWithEncodingsT<std::wstring_view, std::string>( input, base::kWideStringEncoding, base::kNarrowStringEncoding);
    }
    
    std::wstring fromUtf8( const std::string_view& input )
    {
        return base::STLStringToSTLStringWithEncodingsT<std::string_view, std::wstring>( input, base::kNarrowStringEncoding, base::kWideStringEncoding);
    }
}
