// ============================================================================

template <typename StreamDeserializer>
vlt::File_Deserializer<StreamDeserializer>::File_Deserializer(std::filebuf && _file) :
	file(std::move(_file)),
	stream(&file),
	inner_deserializer(stream)
{
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
vlt::File_Deserializer<StreamDeserializer>::~File_Deserializer()
{
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
bool vlt::File_Deserializer<StreamDeserializer>::is_valid() const
{
	return inner_deserializer.is_valid();
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
std::unique_ptr<vlt::Deserializer> vlt::File_Deserializer<StreamDeserializer>::enter_segment(const char * label)
{
	return inner_deserializer.enter_segment(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
const char * vlt::File_Deserializer<StreamDeserializer>::next_label() const
{
	return inner_deserializer.next_label();
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
bool vlt::File_Deserializer<StreamDeserializer>::get_b8(const char * label)
{
	return inner_deserializer.get_b8(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
u32 vlt::File_Deserializer<StreamDeserializer>::get_u32(const char * label)
{
	return inner_deserializer.get_u32(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
s32 vlt::File_Deserializer<StreamDeserializer>::get_s32(const char * label)
{
	return inner_deserializer.get_s32(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
r32 vlt::File_Deserializer<StreamDeserializer>::get_r32(const char * label)
{
	return inner_deserializer.get_r32(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
r64 vlt::File_Deserializer<StreamDeserializer>::get_r64(const char * label)
{
	return inner_deserializer.get_r64(label);
}

// ----------------------------------------------------------------------------

template <typename StreamDeserializer>
const char * vlt::File_Deserializer<StreamDeserializer>::get_string(const char * label)
{
	return inner_deserializer.get_string(label);
}

// ============================================================================
